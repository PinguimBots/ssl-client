#include "pipeline.hpp"
#include "preprocess.hpp"
#include "segmentation.hpp"
#include "extraction.hpp"
#include "contours.hpp"

#include "pinguim/vsss/game_info.hpp"

#include <opencv2/core.hpp>
#include <vector>

void Pipeline::execute(pinguim::vsss::game_info& gi, cv::Mat currframe)
{
    PreProcess::execute(currframe, currframe);

    objectsContours contours = FullSeg::execute(currframe);

    Extraction::execute(gi, contours);
}