
#include "../../include/ModelEstimation/inference.hpp"

// Ultralytics 🚀 AGPL-3.0 License - https://ultralytics.com/license

Inference::Inference(const std::string &onnxModelPath, const cv::Size &modelInputShape, const std::string &classesTxtFile, const bool &runWithCuda)
{
    modelPath = onnxModelPath;
    modelShape = modelInputShape;
    classesPath = classesTxtFile;
    cudaEnabled = runWithCuda;

    loadOnnxNetwork();
    // loadClassesFromFile(); The classes are hard-coded for this example
}

std::vector<Detection> Inference::runInference(const cv::Mat &input)
{
    // 1️⃣ Preprocess image (letterbox)
    cv::Mat modelInput = input;
    int pad_x = 0, pad_y = 0;
    float scale = 1.0f;
    if (letterBoxForSquare && modelShape.width == modelShape.height)
        modelInput = formatToSquare(modelInput, &pad_x, &pad_y, &scale);

    // 2️⃣ Create blob
    cv::Mat blob;
    cv::dnn::blobFromImage(modelInput, blob, 1.0/255.0, modelShape, cv::Scalar(), true, false);
    net.setInput(blob);

    // 3️⃣ Forward pass
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    // 4️⃣ Determine YOLOv8 shape
    cv::Mat out = outputs[0];
    int batch = out.size[0];
    int numDims = out.size[1];   // usually 85 (x,y,w,h + 81 classes)
    int numBoxes = out.size[2];  // e.g., 8400

    bool yolov8 = numDims > numBoxes;
    if (yolov8)
    {
        out = out.reshape(1, numDims); // flatten dimensions
        cv::transpose(out, out);       // shape: (numBoxes, numDims)
    }

    float* data = (float*)out.data;
    int rows = out.rows;
    int dimensions = out.cols;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    // 5️⃣ Loop through all detections
    for (int i = 0; i < rows; ++i)
    {
        float x = data[i * dimensions + 0];
        float y = data[i * dimensions + 1];
        float w = data[i * dimensions + 2];
        float h = data[i * dimensions + 3];

        // class scores start at index 4
        float* scores_ptr = data + i * dimensions + 4;
        cv::Mat scores(1, classes.size(), CV_32FC1, scores_ptr);
        cv::Point class_id;
        double maxClassScore;
        cv::minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

        if (maxClassScore > modelScoreThreshold)
        {
            confidences.push_back((float)maxClassScore);
            class_ids.push_back(class_id.x);

            int left = int((x - 0.5 * w - pad_x) / scale);
            int top = int((y - 0.5 * h - pad_y) / scale);
            int width = int(w / scale);
            int height = int(h / scale);

            boxes.push_back(cv::Rect(left, top, width, height));
        }
    }

    // 6️⃣ Non-Maximum Suppression
    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);

    // 7️⃣ Build detection output
    std::vector<Detection> detections;
    for (int idx : nms_result)
    {
        Detection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];
        result.className = classes[result.class_id];

        // random color for visualization
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(100, 255);
        result.color = cv::Scalar(dis(gen), dis(gen), dis(gen));

        result.box = boxes[idx];
        detections.push_back(result);
    }

    return detections;
}


void Inference::loadClassesFromFile()
{
    std::ifstream inputFile(classesPath);
    if (inputFile.is_open())
    {
        std::string classLine;
        while (std::getline(inputFile, classLine))
            classes.push_back(classLine);
        inputFile.close();
    }
}

void Inference::loadOnnxNetwork()
{
    net = cv::dnn::readNetFromONNX(modelPath);
    if (cudaEnabled)
    {
        std::cout << "\nRunning on CUDA" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    }
    else
    {
        std::cout << "\nRunning on CPU" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
}

cv::Mat Inference::formatToSquare(const cv::Mat &source, int *pad_x, int *pad_y, float *scale)
{
    int col = source.cols;
    int row = source.rows;
    int m_inputWidth = modelShape.width;
    int m_inputHeight = modelShape.height;

    *scale = std::min(m_inputWidth / (float)col, m_inputHeight / (float)row);
    int resized_w = col * *scale;
    int resized_h = row * *scale;
    *pad_x = (m_inputWidth - resized_w) / 2;
    *pad_y = (m_inputHeight - resized_h) / 2;

    cv::Mat resized;
    cv::resize(source, resized, cv::Size(resized_w, resized_h));
    cv::Mat result = cv::Mat::zeros(m_inputHeight, m_inputWidth, source.type());
    resized.copyTo(result(cv::Rect(*pad_x, *pad_y, resized_w, resized_h)));
    resized.release();
    return result;
}