#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    // Open the default camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open camera\n";
        return 1;
    }

    // Set resolution to 320x240
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);

    while (true) {
        // Capture frame from camera
        cv::Mat frame;
        cap >> frame;

        // Calculate width of each part
        int part_width = frame.cols / 10;

        // Convert the frame to HSV color space
        cv::Mat hsv;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        // Initialize variables to store the coordinates of the lowermost red and green objects
        int lowermost_red_x = -1;
        int lowermost_red_y = -1;
        int lowermost_green_x = -1;
        int lowermost_green_y = -1;

        // Process each part separately
        for (int i = 0; i < 10; ++i) {
            // Define the ranges for red and green colors in HSV
            cv::Scalar lower_red = cv::Scalar(0, 100, 100);
            cv::Scalar upper_red = cv::Scalar(10, 255, 255);
            cv::Scalar lower_green = cv::Scalar(50, 100, 100);
            cv::Scalar upper_green = cv::Scalar(70, 255, 255);

            // Crop the current part of the frame
            cv::Mat part = hsv(cv::Rect(i * part_width, 0, part_width, frame.rows));

            // Threshold the HSV frame to get binary masks for red and green regions
            cv::Mat mask_red, mask_green;
            cv::inRange(part, lower_red, upper_red, mask_red);
            cv::inRange(part, lower_green, upper_green, mask_green);

            // Find contours in the binary masks
            std::vector<std::vector<cv::Point>> contours_red, contours_green;
            cv::findContours(mask_red, contours_red, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            cv::findContours(mask_green, contours_green, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            // Find the lowermost red object in the current part
            for (const auto& contour : contours_red) {
                // Compute the centroid of the contour
                cv::Moments m = cv::moments(contour);
                if (m.m00 != 0) {
                    int cx = m.m10 / m.m00 + i * part_width;
                    int cy = m.m01 / m.m00;

                    // Update lowermost_red_x and lowermost_red_y if the current object is lower
                    if (lowermost_red_y == -1 || cy > lowermost_red_y) {
                        lowermost_red_x = cx;
                        lowermost_red_y = cy;
                    }
                }
            }

            // Find the lowermost green object in the current part
            for (const auto& contour : contours_green) {
                // Compute the centroid of the contour
                cv::Moments m = cv::moments(contour);
                if (m.m00 != 0) {
                    int cx = m.m10 / m.m00 + i * part_width;
                    int cy = m.m01 / m.m00;

                    // Update lowermost_green_x and lowermost_green_y if the current object is lower
                    if (lowermost_green_y == -1 || cy > lowermost_green_y) {
                        lowermost_green_x = cx;
                        lowermost_green_y = cy;
                    }
                }
            }
        }

        // Print the difference between the x-coordinates of the green object and the lowermost red object
        if (lowermost_red_x != -1 && lowermost_red_y != -1 && lowermost_green_x != -1 && lowermost_green_y != -1) {
            int x_diff = lowermost_red_x - lowermost_green_x;
            std::cout << "Difference between Green and Red X-coordinates: " << x_diff << std::endl;
        }

        // Highlight the lowermost red object
        if (lowermost_red_x != -1 && lowermost_red_y != -1) {
            cv::circle(frame, cv::Point(lowermost_red_x, lowermost_red_y), 10, cv::Scalar(0, 0, 255), 2);
        }

        // Highlight the lowermost green object
        if (lowermost_green_x != -1 && lowermost_green_y != -1) {
            cv::circle(frame, cv::Point(lowermost_green_x, lowermost_green_y), 10, cv::Scalar(0, 255, 0), 2);
        }

        // Draw a line between the lowermost red and green objects
        if (lowermost_red_x != -1 && lowermost_red_y != -1 && lowermost_green_x != -1 && lowermost_green_y != -1) {
            cv::line(frame, cv::Point(lowermost_red_x, lowermost_red_y), cv::Point(lowermost_green_x, lowermost_green_y), cv::Scalar(255, 0, 0), 2);
        }

        // Display the frame
        cv::imshow("Frame", frame);

        // Check for key press to exit the loop
        if (cv::waitKey(30) >= 0) {
            break;
        }
    }

    // Release the camera
    cap.release();
    cv::destroyAllWindows();

    return 0;
}

