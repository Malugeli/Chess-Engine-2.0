#include "perft.hpp"
#include "movegen.hpp"

uint64_t perft(Board& b, int depth){
    Color us = b.get_gamestate().side_to_move;
    uint64_t move_count{};
    MoveList list;
    generate_moves(b, list);

    for(auto move : list.moves){
        b.do_move(move);
        const Color enemy =  b.get_gamestate().side_to_move;
        Square king_square = b.get_king_square(us);
        if(!is_square_attacked(b, king_square, enemy)){
            move_count += perft(b, depth - 1);
        }
        b.undo_move(move);
    }
    return move_count; 
}
