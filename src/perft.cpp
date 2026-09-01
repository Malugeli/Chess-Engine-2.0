#include "perft.hpp"

#include <algorithm>
#include <print>

#include "debug.hpp"
#include "movegen.hpp"

uint64_t perft(Board& b, int depth){
    if(depth == 0){
        return 1;
    }
    Color us = b.get_gamestate().side_to_move;
    uint64_t move_count{};
    MoveList list;
    generate_moves(b, list);

    for(auto move : list){
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

std::vector<PerftDivideEntry> perft_divide(Board& b, int depth){
    std::vector<PerftDivideEntry> result;
    if(depth <= 0){
        return result;
    }
    const Color us = b.get_gamestate().side_to_move;
    MoveList list;
    generate_moves(b, list);

    for(auto move : list){
        b.do_move(move);
        const Color enemy = b.get_gamestate().side_to_move;
        const Square king_square = b.get_king_square(us);
        if(!is_square_attacked(b, king_square, enemy)){
            result.push_back(PerftDivideEntry{move, perft(b, depth - 1)});
        }
        b.undo_move(move);
    }

    std::ranges::sort(result, [](const PerftDivideEntry& a, const PerftDivideEntry& c){
        return move_to_uci(a.move) < move_to_uci(c.move);
    });
    return result;
}

uint64_t print_perft_divide(Board& b, int depth){
    const auto entries = perft_divide(b, depth);
    uint64_t total{};

    for(const auto& e : entries){
        std::println("{}: {}", move_to_uci(e.move), e.nodes);
        total += e.nodes;
    }
    std::println("");
    std::println("Nodes searched: {}", total);
    return total;
}
