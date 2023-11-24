#include "ProgressBar.h"

ProgressBar::ProgressBar(uint16_t widget_ID, TFT_eSPI &tft) : IWidget(widget_ID, tft) {}

ProgressBar::~ProgressBar() {}

void ProgressBar::setMax(uint16_t max)
{
    if (max < 1)
        _max = 1;
    else
        _max = max;

    _is_changed = true;
}

void ProgressBar::setProgress(uint16_t progress)
{
    if (progress > _max)
        _progress = _max;
    else if (progress < 1)
        _progress = 1;
    else
        _progress = progress;

    _is_changed = true;
}

ProgressBar *ProgressBar::clone(uint16_t id) const
{
    ProgressBar *clone = new ProgressBar(*this);
    clone->_id = id;
    return clone;
}

void ProgressBar::onDraw()
{
    if (!_is_changed)
        return;

    _is_changed = false;

    // if(orientation == horizontal) {

    uint16_t progressW = ((float)_width / _max) * _progress;
    if (progressW < 3)
        progressW = 3;

    if (!_is_first_draw)
    {
        if (_progress > _prev_progress) // Заливка тільки прогресу
        {
            uint16_t next_prgrs_pos = ((float)_width / _max) * _prev_progress;
            _tft.fillRect(next_prgrs_pos + _x_pos - 1, _y_pos + 1, progressW - next_prgrs_pos, _height - 2, _progress_color);
        }
        else if (_progress < _prev_progress) // Заливка тільки фону
        {
            uint16_t prew_progress = ((float)_width / _max) * _prev_progress;
            _tft.fillRect(_x_pos + 1 + progressW - 2, _y_pos + 1, prew_progress - progressW, _height - 2, _back_color);
        }
    }
    else
    {
        _tft.drawRect(_x_pos, _y_pos, _width, _height, _border_color);                                       // рамка
        _tft.fillRect(_x_pos + 1, _y_pos + 1, progressW - 2, _height - 2, _progress_color);                  // прогрес
        _tft.fillRect(_x_pos + 1 + progressW - 2, _y_pos + 1, _width - progressW, _height - 2, _back_color); // фон

        _is_first_draw = false;
    }

    // }else{ } // orientation == vertical

    _prev_progress = _progress;
}

void ProgressBar::reset()
{
    _progress = 1;
    _tft.fillRect(_x_pos + 1, _y_pos + 1, _width - 2, _height - 2, _back_color);
}
