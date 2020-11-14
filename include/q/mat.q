system "d .mat";

enum:{:`int$(x?y)};

types.list:`pawn`knight`bishop`rook`queen`king`;
types.abbr:"PNBRQK ";
types.enum:enum[types.list];
types.abbr2type:{types.list types.abbr?x};
types.type2abbr:{types.abbr types.list?x};

pieces.list:(`white_pawn_a`white_pawn_b`white_pawn_c`white_pawn_d`white_pawn_e`white_pawn_f`white_pawn_g`white_pawn_h`white_knight_b`white_knight_g`white_bishop_c`white_bishop_f`white_rook_a`white_rook_h`white_queen_d`white_king_e`black_pawn_a`black_pawn_b`black_pawn_c`black_pawn_d`black_pawn_e`black_pawn_f`black_pawn_g`black_pawn_h`black_knight_b`black_knight_g`black_bishop_c`black_bishop_f`black_rook_a`black_rook_h`black_queen_d`black_king_e`);
pieces.enum:{if[null x;:-1i];enum[pieces.list;x]}; // NULL sym => -1, white_pawn_a => 0
pieces.enum2type:{if[x<0 | x>31;:last types.list]; v:x mod 16; :types.list ((v div 8) + (v div 10) + (v div 12) + (v div 14) + (v div 15))};
pieces.piece2type:{:pieces.enum2type[pieces.enum[x]]};

color.sym:{if[null x;:`]; @[01b!(`white;`black);pieces.enum[x]>=16]};
color.char:{if[null x;:" "]; @[01b!("wb");pieces.enum[x]>=16]};

ranks.list:raze string[1+til 8];
ranks.enum:enum[ranks.list];

files.list:8#.Q.a;
files.enum:enum[files.list];

roster.size:33; // One extra for null piece
roster.start_coords:((1,0);(1,1);(1,2);(1,3);(1,4);(1,5);(1,6);(1,7);(0,1);(0,6);(0,2);(0,5);(0,0);(0,7);(0,3);(0,4);(6,0);(6,1);(6,2);(6,3);(6,4);(6,5);(6,6);(6,7);(7,1);(7,6);(7,2);(7,5);(7,0);(7,7);(7,3);(7,4);(0N,0N));
roster.data:pieces.list,'(pieces.piece2type each pieces.list),'@[roster.size#0b;roster.size-1;not];
roster.tab:([piece:roster.data[::;0]] t:roster.data[::;1]; c:roster.data[::;2]);
roster.reset:{![`.mat.roster.tab;enlist(not;(null;`piece));0b;`t`c!((pieces.piece2type each;`piece);0b)];};
roster.promote:{[piece;tenum] nt:enlist types.list[tenum]; ![`.mat.roster.tab;enlist(=;`piece;enlist piece);0b;enlist[`t]!enlist[nt]]};

board.grid:8#enlist[til[8]];
board.coords:raze til[8],''board.grid;
board.tab:([r:`s#board.coords[::;0]; f:`g#board.coords[::;1]] piece:`.mat.roster.tab$64#`);
board.clear:{![`.mat.board.tab;();0b;enlist[`piece]!enlist[`.mat.roster.tab$`]]};
board.reset:{board.clear[]; {![`.mat.board.tab;((=;`r;x);(=;`f;y));0b;enlist[`piece]!enlist[`.mat.roster.tab$z]]} ./:  -1_roster.start_coords,'pieces.list;};
board.move:{[piece;dst]
    ![`.mat.board.tab;enlist(=;`piece;enlist piece);0b;enlist[`piece]!enlist[enlist`]];
    ![`.mat.board.tab;((=;`r;dst[0]);(=;`f;dst[1]));0b;enlist[`piece]!enlist[enlist piece]]};

board.capture:{[piece]![`.mat.roster.tab;enlist(=;`piece;enlist piece);0b;enlist[`c]!enlist[1b]]};
board.castle_rook:{[rook;c]
    r:$[16<=pieces.enum[rook];0;7];
    f:(-2*c)+(c<0);
    board.move[rook;(r,f)]};
board.update:{[data]
    dst:data[`dst]; piece:data[`piece];
    // If capturing, mark captured piece as such
    if[data[`capture]; board.capture[?[`.mat.board.tab;((=;`r;dst[0]);(=;`f;dst[1]));();(value;(first;`piece))]]];
    // Move
    board.move[piece;dst];
    // If castling, move rook
    if[c:data[`castle];
        rook:(`white_rook_h`black_rook_h;`white_rook_a`black_rook_a)[c<0][data[`bply]];
        board.castle_rook[rook;c]];
    // If pawn promo, update piece type
    if[p:data[`promo]; roster.promote[piece;p]]};

print.spaces:{x#" "};
print.piece:{color.char[x], types.type2abbr[pieces.piece2type[x]]};
print.square:{[r;f]"[", ?[`.mat.board.tab;((=;`r;r);(=;`f;f));();(print.piece;(value;(first;`piece)))], "]"};
print.header:enlist[l,(l:print.spaces[3]) sv enlist'[files.list]];
print.margin:|:[ranks.list],\:print.spaces[1];
print.board:{
    board:,/'[8 cut print.square ./: ,/[|:[til[8],''board.grid]]];
    :print.header,print.margin,'board};

lookup.name_from_coords:{[r;f]:value board.tab[r,f][`piece]};
lookup.coords_from_name:{[piece]:(,/)?[`.mat.board.tab;enlist(=;`piece;enlist piece);();`r`f!(`..int;)'[`r`f]]};

new_game:{roster.reset[];board.reset[]};

system "d .";