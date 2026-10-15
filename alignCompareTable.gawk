BEGIN {
    inrecord = 0;
    alignment=""
    type=""
    threads=""

    for (i = 0; i <= 29; i++) {
        accum["Mutex", "LocksPerSecond", "NoAlign", i] = 0
        accum["Mutex", "User", "NoAlign", i] = 0
        accum["Mutex", "System", "NoAlign", i] = 0

        accum["FastSpinLock", "LocksPerSecond", "NoAlign", i] = 0
        accum["FastSpinLock", "User", "NoAlign", i] = 0
        accum["FastSpinLock", "System", "NoAlign", i] = 0

        # Safety valve
        accum["unsupported", "LocksPerSecond", "NoAlign", i] = 0
        accum["unsupported", "User", "NoAlign", i] = 0
        accum["unsupported", "System", "NoAlign", i] = 0
    }
}
{
    if (/^ALIGNMENT/) {
        alignment=$2
    } else if (/^BEGIN TEST/) {
        inrecord = 1
    } else if (/^Running /) {
        threads=$6
        if ($4 == 1) {
            type="Mutex"
        } else if ($4 == 2) {
            type="FastSpinLock"
        } else {
            type="unsupported"
        }
    } else if (/^.*StdMutex/) {
        accum[type, "LocksPerSecond", alignment, threads] += $5
    } else if (/^.*MyLock/) {
        accum[type, "LocksPerSecond", alignment, threads] += $5
    } else if (/^user/) {
        accum[type, "User", alignment, threads] += $2
    } else if (/^sys/) {
        accum[type, "System", alignment, threads] += $2
    }
}
END {
    for (i = 0; i <= 29; i++) {
        align=           "style=\"text-align: right;\"";
        normal=          "";
        rightBold=       "style=\"font-weight:bold;\"";
        betterThanNormal="style=\"color: rgb(0,255,0);\"";
        worseThanNormal= "style=\"color: rgb(255,0,0);\"";

        blackCircle=     "\342\232\253";
        greenCircle=     "\360\237\237\242";
        redCircle=       "\360\237\224\264";

        # First row is the actual data
        printf("\t<tr>\n");

            printf("\t    <!-- Threads -->\n");
            printf("\t        <td %s><span %s> %d</span></td>\n", align, rightBold, i);

            printf("\t    <!-- Mutex Locks / Second -->\n");
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, accum["Mutex", "LocksPerSecond", "NoAlign", i] / 10.0);

            printf("\t    <!-- Mutex User+System -->\n");
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, (accum["Mutex", "User", "NoAlign", i] + accum["Mutex", "System", "NoAlign", i]) / 10.0);

            printf("\t    <!-- FastSpinLock Locks / Second -->\n");
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, accum["FastSpinLock", "LocksPerSecond", "NoAlign", i] / 10.0);

            printf("\t    <!-- FastSpinLock User+System -->\n");
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, (accum["FastSpinLock", "User", "NoAlign", i] + accum["FastSpinLock", "System", "NoAlign", i]) / 10.0);

        printf("\t</tr>\n");

        # Second row has the % vs the NoAlign std::mutex case
        printf("\t<tr>\n");

            printf("\t    <!-- Threads -->\n");
            printf("\t        <td %s><span %s>pct</span></td>\n", align, normal);

            printf("\t    <!-- Mutex Locks / Second -->\n");
                mtxLpsNoAlign=accum["Mutex", "LocksPerSecond", "NoAlign", i];
                printf("\t        <td %s><span %s></span></td>\n", align, normal);

            #cpu = (new / old) * 100.0
            printf("\t    <!-- Mutex User+System -->\n");
                mtxUTNoAlign=accum["Mutex", "User", "NoAlign", i];
                mtxSTNoAlign=accum["Mutex", "System", "NoAlign", i];
                mtxTimeNoAlign=mtxUTNoAlign + mtxSTNoAlign;

                printf("\t        <td %s><span %s></span></td>\n", align, normal);

            #lps = (new / old) * 100.0
            printf("\t    <!-- FastSpinLock Locks / Second -->\n");
                fslLpsNoAlign=accum["FastSpinLock", "LocksPerSecond", "NoAlign", i];
                fslLpsNoAlignPct=fslLpsNoAlign/mtxLpsNoAlign;
                fslLpsNoAlignPct *= 100.0;
                if (fslLpsNoAlignPct > 105.0) { data=betterThanNormal; circle=greenCircle; }
                else if (fslLpsNoAlignPct < 95.0) { data=worseThanNormal; circle=redCircle; }
                else { data=normal; circle=blackCircle; }
                printf("\t        <td %s><span %s> %.3f %s</span></td>\n", align, data, fslLpsNoAlignPct, circle);

            #cpu = (new / old) * 100.0
            printf("\t    <!-- FastSpinLock User+System -->\n");
                fslUTNoAlign=accum["FastSpinLock", "User", "NoAlign", i];
                fslSTNoAlign=accum["FastSpinLock", "System", "NoAlign", i];
                fslTimeNoAlign=fslUTNoAlign + fslSTNoAlign;

                fslTimeNoAlignPct=fslTimeNoAlign/mtxTimeNoAlign;
                fslTimeNoAlignPct *= 100.0;
                if (fslTimeNoAlignPct > 105.0) { data=worseThanNormal; circle=redCircle; }
                else if (fslTimeNoAlignPct < 95.0) { data=betterThanNormal; circle=greenCircle; }
                else { data=normal; circle=blackCircle; }
                printf("\t        <td %s><span %s> %.3f %s</span></td>\n", align, data, fslTimeNoAlignPct, circle);

        printf("\t</tr>\n");
    }
}
