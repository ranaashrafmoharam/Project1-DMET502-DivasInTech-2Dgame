#include "LaneHighlight.h"
#include "../world/LaneGrid.h"

void LaneHighlight::draw() {
    LaneGrid::instance().draw();
}
