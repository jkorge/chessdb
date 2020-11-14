file:hsym`$raze .Q.opt[.z.x]`file;
/ file: `:/Users/jkorg/Desktop/WIP/chess/data/otb_samples/otb_samples.pgn;
/ file: `:/Users/jkorg/Desktop/WIP/chess/data/otbgames/OTB.pgn;
/ file: `:/Users/jkorg/Desktop/WIP/chess/data/otbgames/samples/otb_5.pgn;

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
deps:`log.q`parse.q;
load_dep each ` sv/: load_from,'deps;

.tags.keys:`event`site`date`round`white`black`result`eco`fen`mode`timecontrol`termination`whiteelo`blackelo`whiteuscf`blackuscf;
.tags.cols:`event`site`year`month`day`round`white`black`result`eco`fen`mode`timecontrol`termination`whiteelo`blackelo`whiteuscf`blackuscf;
.tags.dict:{(`$lower string x)!y};
.tags.parse:{
    f:.tags.dict . "S ]" 0: ssr[;"[[]";""] x where not x in "\"";
    r:@[f;.tags.keys];
    :(.tags.keys)!r};
.tags.fix.dates:{[tab]
    if[count &:[l:not #:'[?[tab;();();`date]]]; ![tab;enlist l;0b;(enlist`date)!enlist(#;count where l;(enlist;".."))]];
    ![tab;();0b;`year`month`day!?[tab;();();(flip;($/:;"I";(vs/:;".";`date)))]];
    ![tab;();0b;enlist`date]};
.tags.fix.results:{[tab] ![tab;();0b;(enlist`result)!enlist((enlist["*"];"0-1";"1-0";"1/2-1/2")!(0 -1 1 2i);`result)]};
.tags.fix.ratings:{[tab] ![tab;();0b;c!($;"I";) each c:`whiteelo`blackelo`whiteuscf`blackuscf]};
.tags.fix.qmarks:{[tab;c]![tab;enlist(=;c;enlist[`$"?"]);0b;enlist[c]!enlist[enlist`]]};
.tags.fix.syms:{[tab] ![tab;();0b;c!($;enlist`;)each c:`event`site`round`white`black`eco`timecontrol`termination]};
.tags.fix.table:{[tab] @\:[(.tags.fix.dates;.tags.fix.results;.tags.fix.ratings;.tags.fix.syms);tab]; .tags.fix.qmarks[tab;]each ?[meta tab;enlist(=;`t;"s");();`c]};

.mov.parse:{[mt]
    // Replace elided plies with underscores
    mt:{{ssr[x;"...";".__ "]} each x} each mt;
    // Concatenate movetext lines for each game
    mt:raze each {.[y;enlist x;,[;" "]]} ./: (enlist each where each not .[mt;(::;::);last] in\: ". "),'enlist each mt;
    mt:.[mt;enlist where not "." in/: mt; {".",x}];
    :l @' ss\:[.[l:raze each 2#/:/:" " vs/:/: 1_/:"." vs/: mt;(::;::;0)];"[^0-9*]"]};

.res.movetext:();
.res.tagtable:flip .tags.cols!count[.tags.cols]#();
.res.tagtable:.tags.cols xcols .res.tagtable;

.tmp.reset:{.tmp.m:(); .tmp.t:0#.res.tagtable};
.tmp.reset[];

.load.prev_end:`;
.load.N:0;
.load.P:0;
.load.partition:0;
.load.print_every:10000;
.load.dump_every:100000;

.load.pare_dict:{k!v k:where bool[count'[v]]};

.load.dump:{

    // Backup incomplete data
    if[not null .load.prev_end;
        .log.info["Backing up incomplete records";()];
        .bak.both:.load.prev_end=`move;
        .bak.mov:@[`.res.movetext;.load.N-1];
        .bak.tag:@[`.res.tagtable;.load.N-1];
        // If tags of next game still to be processed - reserve last row of tagtable
        `.res.tagtable set @[`.res.tagtable;til .load.N-1];
        // If moves of next game still to be processed - reserve tags and partial movetext
        if[.bak.both;`.res.movetext set @[`.res.movetext;til .load.N-1]]
    ];

    // Tabulate move data and save game number of each tag
    .log.info["Tabulating movetext";()];
    .parse.games[`.res.movetext;count[.res.movetext];`.res.games];
    ![`.res.tagtable;();0b;enlist[`g]!enlist[`i]];

    // Save to disk
    .log.info["Writing tables to disk";()];
    .Q.dpft[`:/data/kdb;.load.partition;`piece;`.res.games];
    .Q.dpft[`:/data/kdb;.load.partition;`event;`.res.tagtable];

    // Clear res containters
    .log.info["Clearing memory";()];
    .res.movetext:();
    ![`.res.tagtable;();0b;`$()];

    // Restore backups
    if[not null .load.prev_end;
        .log.info["Restoring backups";()];
        ![`.res.tagtable;();0b;.bak.tag];
        if[.bak.both; `.res.movetext set enlist[.bak.move]];
    ];

    .load.partition+:1;};

.load.stitch:{[src;dst;n]
    src:n#src;
    dst,:src};

.load.main:{[x]

    // What does this next block start/end on? Movetext, tags, or neither?
    ns:?[not count first x;`;?[.[x;0 0]="[";`tag;`move]];
    ne:?[not count last x;`;?[@[last x;0]="[";`tag;`move]];

    x:enlist[""],x;
    p:1_/:(where not count each x) _ x;

    // Are the movetext chunks on even or odd indices?
    s:?[.[p;0 0 0]~"[";`tag;`move];
    e:?[.[last p;0 0]~"[";`tag;`move];

    l:(til count p) mod 2;

    // Isolate and process movetext
    .tmp.m:p where $[s=`move;not;] l;
    .tmp.m:.mov.parse .tmp.m;

    // Isolate and process tags
    .tmp.t:raze each p where $[s=`tag;not;] l;
    .tmp.t:.tags.parse each .tmp.t where "b"$count each .tmp.t;

    // Munge data and reorder columns
    .tags.fix.table[`.tmp.t];
    .tmp.t:.tags.cols xcols .tmp.t;

    // Append first of current results to last of previous results (as needed)
    if[(not .load.prev_end=`) & (not ns=`);
        $[
        (.load.prev_end=`tag) & (ns=`tag);
            [
                .res.tagtable,:1#.tmp.t;
                .tmp.t _: 0;
            ];
        (.load.prev_end=`move) & (ns=`move);
            [
                ngames:count[.res.movetext];
                @[`.res.movetext;ngames-1;{x,y}[;.tmp.m[0]]];
                .tmp.m _: 0;
            ];

        ]
    ];

    .load.N+:count .tmp.t;
    .load.prev_end:e;

    `.res.tagtable upsert .tmp.t;
    .res.movetext,:.tmp.m;

    .tmp.reset[];
    .Q.gc[];

    if[.load.P<>l:.load.N div .load.print_every;
        .log.info["Count of loaded games";count[.res.tagtable]];
        .load.P:l
    ];

    if[.load.N>=.load.dump_every;
        .log.info["Dumping records"; .load.N];
        .load.dump[];
        .load.N:0
    ]};


.Q.fs[.load.main;file];
/ system"l include/q/parse.q";
/ .res.games:.parse.games .res.movetext;





\

event => sym
site => sym
date => three int columns (year, month, day)
round => sym
white => sym
black => sym
result => int
eco => sym
fen => string
mode => string
timecontrol => sym
termination => sym
whiteelo => int
blackelo => int
whiteuscf => int
blackuscf => int

SSDSSSIIS**SSIIII

`event`site`date`round`white`black`result`eco`fen`mode`timecontrol`termination`whiteelo`blackelo`whiteuscf`blackuscf!"SSDSSSIS**SSIIII"