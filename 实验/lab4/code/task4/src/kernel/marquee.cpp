#include "Marquee.h"
#include "stdio.h"
#include "os_modules.h"

extern STDIO stdio;

Marquee::Marquee() {
    initialize();
}

void Marquee::initialize() {
    text[0] = '2';
    text[1] = '3';
    text[2] = '3';
    text[3] = '3';
    text[4] = '6';
    text[5] = '1';
    text[6] = '7';
    text[7] = '9';
    text[8] = 'M';
    text[9] = 'F';
    text[10] = 'Q';
    text[11] = '\0';

    text_length = 11; // 文本长度更新为11
    position = 0;    // 初始位置（屏幕最左侧）
    screen_width = 80; // 屏幕宽度（列数）
    front_color = 0x0F; // 白色前景
    back_color = 0x10;  // 蓝色背景
    color_counter = 0;
    
    // 清除第一行
    for (int i = 0; i < screen_width; i++) {
        stdio.print(0, i, ' ', back_color);
    }
}

void Marquee::update() {
    // 清除第一行
    for (int i = 0; i < screen_width; i++) {
        stdio.print(0, i, ' ', back_color);
    }
    
    // 显示跑马灯文本
    for (int i = 0; i < text_length; i++) {
        int pos = (position + i) % screen_width;
        if (pos >= 0 && pos < screen_width) {
            // 使用彩色前景
            uint8 color = back_color + ((front_color + i) % 15 + 1);
            stdio.print(0, pos, text[i], color);
        }
    }
    
    // 更新位置 - 实现从左到右的跑马灯效果
    position = (position + 1) % screen_width;
    
    // 定期更改背景色
    color_counter++;
    if (color_counter >= 10) {
        color_counter = 0;
        back_color += 0x10;
        if (back_color >= 0x70) {
            back_color = 0x10;
        }
    }
}
