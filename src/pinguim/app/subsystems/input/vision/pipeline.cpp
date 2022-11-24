#include "pipeline.hpp"
#include "preprocess.hpp"
#include "segmentation.hpp"
#include "extraction.hpp"
#include "contours.hpp"

#include "pinguim/vsss/game_info.hpp"

#include <opencv2/core.hpp>
#include <vector>

cv::Mat Pipeline::execute(pinguim::vsss::game_info& gi, cv::Mat const& currframe, Colors colors)
{
    auto frame = PreProcess::execute(currframe);

    objectsContours contours = FullSeg::execute(frame, colors);

    Extraction::execute(gi, contours);

    return frame;
}