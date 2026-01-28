#ifndef MARQUEE_H
#define MARQUEE_H

#include "os_type.h"

class Marquee {
private:
    // 要显示的文本
    char text[20];
    // 文本长度
    uint8 text_length;
    // 当前位置
    int position;
    // 屏幕宽度
    int screen_width;
    // 前景色
    uint8 front_color;
    // 背景色
    uint8 back_color;
    // 颜色变化计数器
    uint8 color_counter;
public:
    Marquee();
    void initialize();
    // 更新并显示跑马灯
    void update();
};
#endif
