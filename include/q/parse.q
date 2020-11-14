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
deps:(`log.q;`util.q;`mat.q;`disamb.q);
load_dep each ` sv/: load_from,'deps;

.parse.castle:{[src;c] @[src;1;{y+2i*x}[c]]};

.parse.ply:{[ply;black]
    res:`piece`src`dst`castle`capture`check`mate`promo`bply!();
    res[`bply]:black;

    // Check/mate
    res[`check]:"+"=last ply;
    res[`mate]:"#"=last ply;
    if[(res[`check] | res[`mate]); ply:-1_ply];

    // Pawn promotion
    res[`promo]:@[.mat.types.enum;.mat.types.abbr2type first ply[1+where ply="="]] mod 6i;
    if[res[`promo]; ply:-2_ply];

    // Capture
    res[`capture]:"x" in ply;

    // Castle
    res[`castle]:$[ply in c:("O-O-O";"O-O"); (-1i*~:[e])+e:int[c?ply]; 0i];

    // Pre-reqs for disambiguation: piece type, target coords, disambiguating coords (if any)
    $[res[`castle];
        piece_type:`king;
        piece_type:$[any l:(|/)ply=\:.mat.types.abbr; .mat.types.list[first where l]; `pawn]
    ];

    // Extract coords from ply
    files:.mat.files.enum each ply where ply in .mat.files.list;
    files:$[1=n:count[files];-1i,files;not n;(-1 -1i);files];

    ranks:.mat.ranks.enum each ply where ply in .mat.ranks.list;
    ranks:$[1=n:count[ranks];-1i,ranks;not n;(-1 -1i);ranks];

    // PGN plies include target coords (except when castling)
    res[`dst]:last'[(ranks;files)];
    // PGN plies include src coords when needed
    res[`src]:first'[(ranks;files)];

    $[
    // No further disambiguation needed if PGN ply contains full disambiguation (eg. Qh4e1)...
    not -1 in res[`src];
        res[`piece]:.mat.lookup.name_from_coords . res[`src];

    // .. OR if piece_type=king
    piece_type=`king;
        [
            res[`piece]:.disamb.king[black];
            res[`src]:.mat.lookup.coords_from_name[res[`piece]];
            if[res[`castle];
                res[`dst]:.parse.castle[res[`src];res[`castle]]];
        ];

        [
            // Select candidates and disambiguate
            constraint:(.disamb.constraint.not_captured; .disamb.constraint.color[black]; .disamb.constraint.type[piece_type]);
            .disamb.candidates.find[constraint];
            $[piece_type=`pawn;
                res[`piece]:.disamb.pawn[res[`src]; res[`dst]; black; res[`capture]];
                res[`piece]:.disamb.piece[res[`src]; res[`dst];piece_type]
            ];
            res[`src]:.mat.lookup.coords_from_name[res[`piece]];
        ]
    ];
    .mat.board.update[res];
    :res};


.parse.game:{[mt]
    .mat.new_game[];
    turns:int[til[count l]],'enlist'[l:2 cut mt];
    ,/[.parse.turn ./: turns]};

.parse.numbered_game:{[n;mt]update g:n from .parse.game[mt]; .log.info["Finished game";n]};
.parse.peach.numbered_game:.[.parse.numbered_game]':;

.parse.turn:{[n;mt]
    plies:{$[2=count[x];enlist'[x],'01b;enlist[x],'0b]} mt;
    update turn:n from .parse.ply ./: plies};

.parse.games:{[mt;n;tab]
    ngames:int[til[n]];
    (upsert[tab;]':) (.[.parse.numbered_game;]':) flip (ngames;@[mt;::])};





