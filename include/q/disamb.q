proot:`chess;
include:`include;
here:`q;
tree:(proot;include;here);

iswin:.z.o like "w??";
pwd:{hsym `${$[iswin;2_ssr[x;"\\";"/"];x]} system $[iswin;"cd";"pwd"]};
wd:{last ` vs pwd[]};
ls:{`$system $[iswin;"dir/b ";"ls "],$[10<>type x;"";x]};
load_dep:{@[system;"l ",1_string[x]]};

if[not (l:wd[]) in tree; 'wrong_dir];
load_from:` sv @[;0;hsym](1+tree?wd[]) _ tree;
deps:(`util.q;`mat.q);
load_dep each ` sv/: load_from,'deps;

system "d .disamb";

// CONSTRAINTS FOR SELECTING CANDIDATES
constraint.not_captured:(not;`piece.c);
constraint.color:{[black] :(=;(.mat.color.sym each;`piece);enlist$[black;`black;`white])};
constraint.type:{[piece_type] :(=;`piece.t;enlist piece_type)};

// CONSTRAINTS ON CANDIDATES BY MATERIAL TYPE
constraint.pawn:{[capture;one;two;dst] $[capture;:((=;`r;dst[0]-one);(=;(abs;(-;dst[1];`f));1));:((|;(=;`r;(-;dst[0];one));(=;`r;(-;dst[0];two)));(=;`f;dst[1]))]};
constraint.knight:{[dst] :(&;(<>;(=;(abs;(-;dst[0];`r));2);(=;(abs;(-;dst[1];`f));2));(<>;(=;(abs;(-;dst[0];`r));1);(=;(abs;(-;dst[1];`f));1)))};
constraint.bishop:{[dst] :(=;(abs;(-;dst[0];`r));(abs;(-;dst[1];`f)))};
constraint.rook:{[dst] :(<>;(=;0;(-;dst[0];`r));(=;0;(-;dst[1];`f)))};
constraint.queen:{[dst] :(<>;constraint.rook[dst];constraint.bishop[dst])};

// DICT OF PIECE CONSTRAINTS
constraint.piece:{[pt;dst] :@[`knight`bishop`rook`queen!(constraint.knight;constraint.bishop;constraint.rook;constraint.queen);pt][dst]};

// CANDIDATES FOR DISAMBIGUATION
candidates.tab:([r:`..int[()];f:`..int[()]] piece:`..symbol[()]);
candidates.find:{[constraint] candidates.tab:`r`f xkey ?[`.mat.board.tab;constraint;0b;`r`f`piece!(`r;`f;(value;`piece))];};

// FILTER CANDIDATE PIECES
candidates.filter.piece:{[pt;src;dst]
    v:?[`.disamb.candidates.tab;enlist constraint.piece[pt;dst];0b;(`piece`r`f`dr`df)!(`piece;`r;`f;(-;dst[0];`r);(-;dst[1];`f))];
    if[not all l:-1=src; l:where not l; v:?[v;enlist(=;first `r`f @ l;first src @ l);0b;()]];
    :v};

// FILTER CANDIDATE PAWNS
candidates.filter.pawn:{[src;dst;black;capture]
    one:1 - (2*black);
    two:2 - (4*black);
    v:?[`.disamb.candidates.tab;constraint.pawn[capture;one;two;dst];0b;(`piece`r`f`dr`df)!(`piece;`r;`f;(-;dst[0];`r);(-;dst[1];`f))];
    if[not all l:-1=src; l:where not l; v:?[v;enlist(=;first `r`f @ l;first src @ l);0b;()]];
    :v};

// UTILS
util.ranges:{$[x;-1 _ l + (l:signum[x]) * til abs[x];0]};
util.obstructed:{any @/:[{?[`.mat.board.tab;((not;`piece.c);(=;`r;x[0]);(=;`f;x[1]));();(count;`i)]};x]}

// DISAMBIGUATION FUNCTIONS
pawn:{[src;dst;black;capture]
    candidates.tab:candidates.filter.pawn[src;dst;black;capture];
    :?[`.disamb.candidates.tab;();();(first;`piece)]};

piece:{[src;dst;pt]
    candidates.tab:candidates.filter.piece[pt;src;dst];
    if[(1=count[candidates.tab]) | (pt=`knight); :?[`.disamb.candidates.tab;();();(first;`piece)]];
    loi:./:[,';flip @/:[?[`.disamb.candidates.tab;();();];{(+;x;(util.ranges each;y))} ./: (`r`dr;`f`df)]];
    :first candidates.tab[`piece] where not @/:[util.obstructed;loi]};

king:{[black]:@[.mat.pieces.list;] .mat.pieces.enum[`white_king_e] + (black * 16)};

system "d .";