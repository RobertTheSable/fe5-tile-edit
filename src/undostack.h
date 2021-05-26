#ifndef UNDOSTACK_H
#define UNDOSTACK_H
#include <stack>
#include <vector>

namespace UndoStack {
    static const int TILESET_EDIT = 0b00000010;
    static const int TILE_EDIT  = 0b000000001;
    static const int PIXEL_EDIT = 0b000000100;
//    static const int ACTIVE_DATA  = 0b100000000;
    struct UndoEntry{
        int data_type;
        unsigned int change_index;
        std::vector<unsigned char> data;
    };
}


#endif // UNDOSTACK_H
