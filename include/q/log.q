
system"c 50 150";
.log.sep:" <> ";
.log.prefix:{[lvl]("[",string[lvl],"]";string[.z.p];string[.z.h];string[.z.i])};
.log.out:{[lvl;str;val]
    $[20<=type val;
        val:.Q.s[val] except "\r\n -";
        val:raze string[val]];
    show[.log.sep sv .log.prefix[lvl],(str;val)]};
.log.info:{[str;val].log.out[`INFO;str;val]};
.log.warn:{[str;val].log.out[`WARN;str;val]};
.log.debug:{[str;val].log.out[`DEBUG;str;val]};
.log.error:{[str;val].log.out[`ERROR;str;val]};


.log.mns:{` sv `,x};
.log.ens:{[ns]
    m:key ns;
    $[(m~ns) | (11h<>abs type m);
        :();
        if[null m[0]; m:1_m]
    ];
    $[count m;
        [
            m:` sv/: ns,/:m;
            nn:ens each m;
            nn:raze nn l:where "b"$count'[nn];
            if[count nn;
                m,:nn];
            :m
        ];
        :()
    ]};


/
now:.z.p;
vals:(0b;01b;first 1?0Ng;2?0Ng;0xaf;0xaf23;0h;0 1h;0i;0 1i;0j;0 1j;0e;0 1e;0f;0 1f;"a";"abc";`a;`a`b;now;now, now;"m"$now;"m"$(now,now);"d"$now;"d"$(now,now);"z"$now;"z"$(now,now);"u"$now;"u"$(now,now);"v"$now;"v"$(now,now);"t"$now;"t"$(now,now))