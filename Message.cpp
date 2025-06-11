/*
File Name : Message.h
Author: 이시행
Purpose: 메세지 타입 및 헤더 처리 함수 정의
Create date : 2025-06-12
*/

#include "Message.h"

// 메시지 헤더 생성 함수
uint32_t make_header(bool is_group, uint32_t msg_length) {
    msg_length &= 0x7FFFFFFF;
    uint32_t header = 0;
    header |= (uint32_t(is_group) << 31);
    header |= msg_length;
    return header;
}

// 메시지 헤더 파싱 함수
void parse_header(uint32_t header, bool& is_group, uint32_t& msg_length)
{
    is_group = (header >> 31) & 0x1;
    msg_length = header & 0x7FFFFFFF;
}