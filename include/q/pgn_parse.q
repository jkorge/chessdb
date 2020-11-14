.mat.enum:{:x?y};

.mat.types.abbr:"PNBRQK ";
.mat.types.list:`pawn`knight`bishop`rook`queen`king`;
.mat.types.enum:.mat.enum[.mat.types.list];
.mat.types.abbr2type:{.mat.types.list .mat.types.abbr?x};
.mat.types.type2abbr:{.mat.types.abbr .mat.types.list?x};

.mat.pieces.list:(`white_pawn_a`white_pawn_b`white_pawn_c`white_pawn_d`white_pawn_e`white_pawn_f`white_pawn_g`white_pawn_h`white_knight_b`white_knight_g`white_bishop_c`white_bishop_f`white_rook_a`white_rook_h`white_queen_d`white_king_e`black_pawn_a`black_pawn_b`black_pawn_c`black_pawn_d`black_pawn_e`black_pawn_f`black_pawn_g`black_pawn_h`black_knight_b`black_knight_g`black_bishop_c`black_bishop_f`black_rook_a`black_rook_h`black_queen_d`black_king_e`);
.mat.pieces.enum:{if[null x;:-1];.mat.enum[.mat.pieces.list;x]}; // NULL sym => -1, white_pawn_a => 0
.mat.pieces.enum2type:{if[x<0 | x>31;:last .mat.types.list]; v:x mod 16; :.mat.types.list ((v div 8) + (v div 10) + (v div 12) + (v div 14) + (v div 15))};
.mat.pieces.enum2piece:{.mat.pieces.list[x]};
.mat.pieces.piece2type:{:.mat.pieces.enum2type[.mat.pieces.enum[x]]};

.mat.color.sym:{if[null x;:`]; @[01b!(`white;`black);.mat.pieces.enum[x]>=16]};
.mat.color.char:{if[null x;:" "]; @[01b!("wb");.mat.pieces.enum[x]>=16]};

.mat.ranks.list:raze string[1+til 8];
.mat.ranks.enum:.mat.enum[.mat.ranks.list];

.mat.files.list:8#.Q.a;
.mat.files.enum:.mat.enum[.mat.files.list];

.mat.roster.size:33; // One extra for null piece
.mat.roster.start_coords:((1,0);(1,1);(1,2);(1,3);(1,4);(1,5);(1,6);(1,7);(0,1);(0,6);(0,2);(0,5);(0,0);(0,7);(0,3);(0,4);(6,0);(6,1);(6,2);(6,3);(6,4);(6,5);(6,6);(6,7);(7,1);(7,6);(7,2);(7,5);(7,0);(7,7);(7,3);(7,4);(0N,0N));
.mat.roster.data:.mat.pieces.list,'(.mat.pieces.piece2type each .mat.pieces.list),'@[.mat.roster.size#0b;.mat.roster.size-1;not];
.mat.roster.tab:([piece:.mat.roster.data[::;0]] t:.mat.roster.data[::;1]; c:.mat.roster.data[::;2]);
.mat.roster.reset:{![`.mat.roster.tab;enlist(not;(null;`piece));0b;`t`c!((.mat.pieces.piece2type each;`piece);0b)]};
.mat.roster.promote:{[piece;p]![`.mat.roster.tab;enlist(=;`piece;enlist piece);0b;enlist[`t]!enlist[.mat.types.list[p]]]};

.mat.board.grid:8#enlist[til[8]];
.mat.board.coords:raze til[8],''.mat.board.grid;
.mat.board.tab:([r:`s#.mat.board.coords[::;0]; f:`g#.mat.board.coords[::;1]] piece:`.mat.roster.tab$64#`);
.mat.board.clear:{![`.mat.board.tab;();0b;enlist[`piece]!enlist[`.mat.roster.tab$`]]};
.mat.board.reset:{.mat.board.clear[]; {![`.mat.board.tab;((=;`r;x);(=;`f;y));0b;enlist[`piece]!enlist[`.mat.roster.tab$z]]} ./:  -1_.mat.roster.start_coords,'.mat.pieces.list;};
.mat.board.move:{[piece;dst]
    ![`.mat.board.tab;enlist(=;`piece;enlist piece);0b;enlist[`piece]!enlist[enlist`]];
    ![`.mat.board.tab;((=;`r;dst[0]);(=;`f;dst[1]));0b;enlist[`piece]!enlist[enlist piece]]};

.mat.board.capture:{[piece]![`.mat.roster.tab;enlist(=;`piece;enlist piece);0b;enlist[`c]!enlist[1b]]};
.mat.board.castle_rook:{[rook;c]
    r:$[16<=.mat.pieces.enum[rook];0;7];
    f:(-2*c)+(c<0);
    .mat.board.move[rook;(r,f)]};

.mat.print.spaces:{x#" "};
.mat.print.piece:{.mat.color.char[x], .mat.types.type2abbr[.mat.pieces.piece2type[x]]};
.mat.print.square:{[r;f]"[", ?[`.mat.board.tab;((=;`r;r);(=;`f;f));();(.mat.print.piece;(value;(first;`piece)))], "]"};
.mat.print.header:enlist[l,(l:.mat.print.spaces[3]) sv enlist'[.mat.files.list]];
.mat.print.margin:|:[.mat.ranks.list],\:.mat.print.spaces[1];
.mat.print.board:{
    board:,/'[8 cut .mat.print.square ./: ,/[|:[til[8],''.mat.board.grid]]];
    :.mat.print.header,.mat.print.margin,'board};

.mat.lookup.name_from_coords:{[r;f]:?[`.mat.board.tab;((not;`piece.c);(=;`r;r);(=;`f;f));();(first;`piece)]};
.mat.lookup.coords_from_name:{[piece]:(,/)?[`.mat.board.tab;enlist(=;`piece;enlist piece);();`r`f!`r`f]};

.mat.board.update:{[data]
    dst:data[`dst]; piece:data[`piece];
    // If capturing, mark captured piece as such
    if[data[`capture]; .mat.board.capture[?[`.mat.board.tab;((=;`r;dst[0]);(=;`f;dst[1]));();(value;(first;`piece))]]];
    // Move
    .mat.board.move[piece;dst];
    // If castling, move rook
    if[c:data[`castle];
        rook:(`white_rook_h`black_rook_h;`white_rook_a`black_rook_a)[c<0][data[`black]];
        .mat.board.castle_rook[rook;c]];
    // If pawn promo, update piece type
    if[p:data[`promo]; .mat.roster.promote[piece;p]]};


// CONSTRAINTS FOR SELECTING CANDIDATES
.disamb.constraint.not_captured:(not;`piece.c);
.disamb.constraint.color:{[black] :(=;(.mat.color.sym each;`piece);enlist$[black;`black;`white])};
.disamb.constraint.type:{[piece_type] :(=;`piece.t;enlist piece_type)};

// CONSTRAINTS ON CANDIDATES BY MATERIAL TYPE
.disamb.constraint.pawn:{[capture;one;two;dst] $[capture;:((=;`r;dst[0]-one);(=;(abs;(-;dst[1];`f));1));:((|;(=;`r;(-;dst[0];one));(=;`r;(-;dst[0];two)));(=;`f;dst[1]))]};
.disamb.constraint.knight:{[dst] :(&;(<>;(=;(abs;(-;dst[0];`r));2);(=;(abs;(-;dst[1];`f));2));(<>;(=;(abs;(-;dst[0];`r));1);(=;(abs;(-;dst[1];`f));1)))};
.disamb.constraint.bishop:{[dst] :(=;(abs;(-;dst[0];`r));(abs;(-;dst[1];`f)))};
.disamb.constraint.rook:{[dst] :(<>;(=;0;(-;dst[0];`r));(=;0;(-;dst[1];`f)))};
.disamb.constraint.queen:{[dst] :(<>;.disamb.constraint.rook[dst];.disamb.constraint.bishop[dst])};

// DICT OF PIECE CONSTRAINTS
.disamb.constraint.piece:{[pt;dst] :@[`knight`bishop`rook`queen!(.disamb.constraint.knight;.disamb.constraint.bishop;.disamb.constraint.rook;.disamb.constraint.queen);pt][dst]};

// FILTER CANDIDATE PIECES
.disamb.filter.piece:{[pt;src;dst]
    v:?[`.disamb.candidates;enlist .disamb.constraint.piece[pt;dst];0b;(`piece`r`f`dr`df)!(`piece;`r;`f;(-;dst[0];`r);(-;dst[1];`f))];
    if[not all l:-1=src; l:where not l; v:?[v;enlist(=;first `r`f @ l;first src @ l);0b;()]];
    :v};

// FILTER CANDIDATE PAWNS
.disamb.filter.pawn:{[src;dst;black;capture]
    one:1 - (2*black);
    two:2 - (4*black);
    v:?[`.disamb.candidates;.disamb.constraint.pawn[capture;one;two;dst];0b;(`piece`r`f`dr`df)!(`piece;`r;`f;(-;dst[0];`r);(-;dst[1];`f))];
    if[not all l:-1=src; l:where not l; v:?[v;enlist(=;first `r`f @ l;first src @ l);0b;()]];
    :v};

// UTILS
.disamb.util.ranges:{$[x;-1 _ l + (l:signum[x]) * til abs[x];0]};
.disamb.util.obstructed:{any @/:[{?[`.mat.board.tab;((not;`piece.c);(=;`r;x[0]);(=;`f;x[1]));();(count;`i)]};x]}

// DISAMBIGUATION FUNCTIONS
.disamb.pawn:{[src;dst;black;capture]
    .disamb.candidates:.disamb.filter.pawn[src;dst;black;capture];
    :?[`.disamb.candidates;();();(first;`piece)]};

.disamb.piece:{[pt;src;dst]
    .disamb.candidates:.disamb.filter.piece[pt;src;dst];
    if[(1=count[.disamb.candidates]) | (pt=`knight); :?[`.disamb.candidates;();();(first;`piece)]];
    loi:./:[,';flip @/:[?[`.disamb.candidates;();();];{(+;x;(.disamb.util.ranges each;y))} ./: (`r`dr;`f`df)]];
    :first .disamb.candidates[`piece] where not @/:[.disamb.util.obstructed;loi]};

.disamb.king:{[black]:.mat.pieces.enum2piece .mat.pieces.enum[`white_king_e] + (black * 16)};

.parse.castle:{[side;black]
    piece:.disamb.king[black];
    piece_type:`king;
    src:.mat.lookup.coords_from_name[piece];
    dst:(src[0];src[1] + $[side=`queen;-2;2]);
    castle:$[side=`queen;-1;1];
    :`piece`src`dst`castle!(piece;src;dst;castle)};

.parse.ply:{[ply;black]
    res:`piece`src`dst`castle`capture`check`mate`promo`black!();
    res[`black]:black;

    // Check/mate
    res[`check]:"+"=last ply;
    res[`mate]:"#"=last ply;
    if[(res[`check] | res[`mate]); ply:-1_ply];

    // Pawn promotion
    res[`promo]:@[.mat.types.enum;.mat.types.abbr2type @[;0]ply[1+where ply="="]] mod 6;
    if[res[`promo]; ply:-2_ply];

    // Capture
    res[`capture]:"x" in ply;

    // Castle
    if[any l:ply~/:("O-O";"O-O-O"); :res,.parse.castle[first `king`queen where l;black]];
    res[`castle]:0;

    // Pre-reqs for disambiguation: piece type, target coords, disambiguating coords (if any)
    piece_type:$[any l:(|/)ply=\:.mat.types.abbr; .mat.types.list[first where l]; `pawn];

    files:.mat.files.enum each ply where ply in .mat.files.list;
    files:$[1=count[files];-1,files;files];

    ranks:.mat.ranks.enum each ply where ply in .mat.ranks.list;
    ranks:$[1=count[ranks];-1,ranks;ranks];

    // PGN plies ALWAYS include target coords
    res[`dst]:last'[(ranks;files)];
    res[`src]:first'[(ranks;files)];

    $[
    // No further disambiguation needed if PGN ply contains full disambiguation (eg. Qh4e1)...
    not -1 in res[`src];
        res[`piece]:.mat.lookup.name_from_coords . res[`src];

    // .. OR if piece_type=king
    piece_type=`king;
        [
            res[`piece]:.disamb.king[black];
            res[`src]:.mat.lookup.coords_from_name[res[`piece]]
        ];

        [
            // Select candidates and disambiguate
            constraint:(.disamb.constraint.not_captured; .disamb.constraint.color[black]; .disamb.constraint.type[piece_type]);
            .disamb.candidates:`r`f xkey ?[`.mat.board.tab;constraint;0b;`r`f`piece!(`r;`f;(value;`piece))];
            $[
                piece_type=`pawn;
                    res[`piece]:.disamb.pawn[res[`src]; res[`dst]; black; res[`capture]];
                    res[`piece]:.disamb.piece[piece_type; res[`src]; res[`dst]]
            ];
            res[`src]:(,/)?[`.mat.board.tab;enlist(=;`piece;(@;res;enlist`piece));();`r`f!`r`f];
        ]
    ];

    .mat.board.update[res];};


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/ movetext:("e4";"c5";"b3";"d6";"Bb2";"Nf6";"Nc3";"a6";"g3";"Nc6";"Bg2";"Qc7";"Nge2";"e6";"d4";"cxd4";"Nxd4";"Be7";"O-O";"O-O";"Nxc6";"bxc6";"f4";"d5";"exd5";"cxd5";"Re1";"Bb7";"Ne2";"Rac8";"Rc1";"Bc5+";"Kh1";"Ng4";"Bd4";"Ba3";"Nc3";"Nf2+";"Bxf2";"Bxc1";"Qxc1";"Qxc3";"Re2";"Rfd8";"Qd1";"Rd7";"Bd4";"Qa5";"a4";"Qc7";"f5";"exf5";"Re3";"Re7";"Rxe7";"Qxe7";"c3";"Re8";"Kg1";"Qe1+";"Qxe1";"Rxe1+";"Kf2";"Rb1";"b4";"g6";"a5";"Kf8";"Bc5+";"Ke8";"c4";"Rb2+";"Kg1";"Rd2";"Kf1";"Kd7";"h4";"Bc6";"cxd5";"Bxd5";"Bxd5";"Rxd5";"Ke2";"Re5+";"Kf3";"Re4";"Bf8";"Kc6");
/ inputs:(enlist each movetext),'"b"$til[count[movetext]] mod 2;
/ .mat.board.reset[];
/ .mat.roster.reset[];
/ show[.mat.print.board[]];
/ show[""];
/ .parse.ply ./: inputs;


