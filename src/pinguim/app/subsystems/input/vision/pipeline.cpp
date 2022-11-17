#include "pipeline.hpp"
#include "preprocess.hpp"
#include "segmentation.hpp"
#include "extraction.hpp"
#include "contours.hpp"

#include "pinguim/vsss/game_info.hpp"

#include <opencv2/core.hpp>
#include <vector>

<<<<<<< HEAD
void Pipeline::execute(pinguim::vsss::game_info& gi, cv::InputArray currframe, Colors colors)
=======
void Pipeline::execute(pinguim::vsss::game_info& gi, cv::Mat& currframe, Colors colors)
>>>>>>> e74e739 (refator(input/vision): reorganize main func)
{
    currframe = PreProcess::execute(currframe);

    objectsContours contours = FullSeg::execute(currframe, colors);

    Extraction::execute(gi, contours);
}