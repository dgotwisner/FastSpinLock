BEGIN {
    inrecord = 0;
    alignment=""
    type=""
    threads=""

    for (i = 0; i <= 29; i++) {

        accum["Mutex", "LocksPerSecond", "NoAlign", i] = 0
        accum["Mutex", "LocksPerSecond", "Align64", i] = 0
        accum["Mutex", "LocksPerSecond", "Align128", i] = 0

        accum["Mutex", "User", "NoAlign", i] = 0
        accum["Mutex", "User", "Align64", i] = 0
        accum["Mutex", "User", "Align128", i] = 0

        accum["Mutex", "System", "NoAlign", i] = 0
        accum["Mutex", "System", "Align64", i] = 0
        accum["Mutex", "System", "Align128", i] = 0

        accum["FastSpinLock", "LocksPerSecond", "NoAlign", i] = 0
        accum["FastSpinLock", "LocksPerSecond", "Align64", i] = 0
        accum["FastSpinLock", "LocksPerSecond", "Align128", i] = 0

        accum["FastSpinLock", "User", "NoAlign", i] = 0
        accum["FastSpinLock", "User", "Align64", i] = 0
        accum["FastSpinLock", "User", "Align128", i] = 0

        accum["FastSpinLock", "System", "NoAlign", i] = 0
        accum["FastSpinLock", "System", "Align64", i] = 0
        accum["FastSpinLock", "System", "Align128", i] = 0

        # Safety valve
        accum["unsupported", "LocksPerSecond", "NoAlign", i] = 0
        accum["unsupported", "LocksPerSecond", "Align64", i] = 0
        accum["unsupported", "LocksPerSecond", "Align128", i] = 0

        accum["unsupported", "User", "NoAlign", i] = 0
        accum["unsupported", "User", "Align64", i] = 0
        accum["unsupported", "User", "Align128", i] = 0

        accum["unsupported", "System", "NoAlign", i] = 0
        accum["unsupported", "System", "Align64", i] = 0
        accum["unsupported", "System", "Align128", i] = 0
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
        betterThanNormal="style=\"color: green;\"";
        worseThanNormal= "style=\"color: red;\"";

        # First row is the actual data
        printf("\t<tr>\n");

            printf("\t    <!-- Threads -->\n");
            printf("\t        <td %s><span %s> %d</span></td>\n", align, rightBold, i);

            printf("\t    <!-- Mutex Locks / Second -->\n");
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, accum["Mutex", "LocksPerSecond", "NoAlign", i] / 10.0);
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, accum["Mutex", "LocksPerSecond", "Align64", i] / 10.0);
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, accum["Mutex", "LocksPerSecond", "Align128", i] / 10.0);

            printf("\t    <!-- Mutex User+System -->\n");
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, (accum["Mutex", "User", "NoAlign", i] + accum["Mutex", "System", "NoAlign", i]) / 10.0);
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, (accum["Mutex", "User", "Align64", i] + accum["Mutex", "System", "Align64", i]) / 10.0);
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, (accum["Mutex", "User", "Align128", i] + accum["Mutex", "System", "Align128", i]) / 10.0);

            printf("\t    <!-- FastSpinLock Locks / Second -->\n");
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, accum["FastSpinLock", "LocksPerSecond", "NoAlign", i] / 10.0);
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, accum["FastSpinLock", "LocksPerSecond", "Align64", i] / 10.0);
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, accum["FastSpinLock", "LocksPerSecond", "Align128", i] / 10.0);

            printf("\t    <!-- FastSpinLock User+System -->\n");
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, (accum["FastSpinLock", "User", "NoAlign", i] + accum["FastSpinLock", "System", "NoAlign", i]) / 10.0);
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, (accum["FastSpinLock", "User", "Align64", i] + accum["FastSpinLock", "System", "Align64", i]) / 10.0);
            printf("\t        <td %s><span %s> %.3f</span></td>\n", align, rightBold, (accum["FastSpinLock", "User", "Align128", i] + accum["FastSpinLock", "System", "Align128", i]) / 10.0);

        printf("\t</tr>\n");

        # Second row has the % vs the NoAlign std::mutex case
        printf("\t<tr>\n");

            #usrSysNoAlign=(accum["Mutex", "User", "NoAlign", i] + accum["Mutex", "System", "NoAlign", i]);
            #lpsAlign64Value=accum["Mutex", "LocksPerSecond", "Align64", i];
            #usrSysAlign64=(accum["Mutex", "User", "Align64", i] + accum["Mutex", "System", "Align64", i]);
            #lpsAlign128Value=accum["Mutex", "LocksPerSecond", "Align128", i];
            #usrSysAlign128=(accum["Mutex", "User", "Align128", i] + accum["Mutex", "System", "Align128", i]);

            printf("\t    <!-- Threads -->\n");
            printf("\t        <td %s><span %s>pct</span></td>\n", align, normal);

            printf("\t    <!-- Mutex Locks / Second -->\n");
                mtxLpsNoAlign=accum["Mutex", "LocksPerSecond", "NoAlign", i];
                mtxLpsAlign64=accum["Mutex", "LocksPerSecond", "Align64", i];
                mtxLpsAlign128=accum["Mutex", "LocksPerSecond", "Align128", i];
                mtxLpsAlign64Pct=mtxLpsAlign64/mtxLpsNoAlign;
                mtxLpsAlign128Pct=mtxLpsAlign128/mtxLpsNoAlign;
                mtxLpsAlign64Pct *= 100.0;
                mtxLpsAlign128Pct *= 100.0;
                if (mtxLpsAlign64Pct > 105.0) { data64=betterThanNormal; }
                else if (mtxLpsAlign64Pct < 95.0) { data64=worseThanNormal; }
                else { data64=normal; }
                if (mtxLpsAlign128Pct > 105.0) { data128=betterThanNormal; }
                else if (mtxLpsAlign128Pct < 95.0) { data128=worseThanNormal; }
                else { data128=normal; }
                printf("\t        <td %s><span %s></span></td>\n", align, normal);
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data64, mtxLpsAlign64Pct);
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data128, mtxLpsAlign128Pct);

            #cpu = (new / old) * 100.0
            printf("\t    <!-- Mutex User+System -->\n");
                mtxUTNoAlign=accum["Mutex", "User", "NoAlign", i];
                mtxUTAlign64=accum["Mutex", "User", "Align64", i];
                mtxUTAlign128=accum["Mutex", "User", "Align128", i];
                mtxSTNoAlign=accum["Mutex", "System", "NoAlign", i];
                mtxSTAlign64=accum["Mutex", "System", "Align64", i];
                mtxSTAlign128=accum["Mutex", "System", "Align128", i];
                mtxTimeNoAlign=mtxUTNoAlign + mtxSTNoAlign;
                mtxTimeAlign64=mtxUTAlign64 + mtxSTAlign64;
                mtxTimeAlign128=mtxUTAlign128 + mtxSTAlign128;

                mtxTimeAlign64Pct=mtxTimeAlign64/mtxTimeNoAlign;
                mtxTimeAlign128Pct=mtxTimeAlign128/mtxTimeNoAlign;
                mtxTimeAlign64Pct *= 100.0;
                mtxTimeAlign128Pct *= 100.0;
                if (mtxTimeAlign64Pct > 105.0) { data64=worseThanNormal; }
                else if (mtxTimeAlign64Pct < 95.0) { data64=betterThanNormal; }
                else { data64=normal; }
                if (mtxTimeAlign128Pct > 105.0) { data128=worseThanNormal; }
                else if (mtxTimeAlign128Pct < 95.0) { data128=betterThanNormal; }
                else { data128=normal; }
                printf("\t        <td %s><span %s></span></td>\n", align, normal);
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data64, mtxTimeAlign64Pct);
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data128, mtxTimeAlign128Pct);

            #lps = (new / old) * 100.0
            printf("\t    <!-- FastSpinLock Locks / Second -->\n");
                fslLpsNoAlign=accum["FastSpinLock", "LocksPerSecond", "NoAlign", i];
                fslLpsAlign64=accum["FastSpinLock", "LocksPerSecond", "Align64", i];
                fslLpsAlign128=accum["FastSpinLock", "LocksPerSecond", "Align128", i];
                fslLpsNoAlignPct=fslLpsNoAlign/mtxLpsNoAlign;
                fslLpsAlign64Pct=fslLpsAlign64/mtxLpsNoAlign;
                fslLpsAlign128Pct=fslLpsAlign128/mtxLpsNoAlign;
                fslLpsNoAlignPct *= 100.0;
                fslLpsAlign64Pct *= 100.0;
                fslLpsAlign128Pct *= 100.0;
                if (fslLpsNoAlignPct > 105.0) { data=betterThanNormal; }
                else if (fslLpsNoAlignPct < 95.0) { data=worseThanNormal; }
                else { data=normal; }
                if (fslLpsAlign64Pct > 105.0) { data64=betterThanNormal; }
                else if (fslLpsAlign64Pct < 95.0) { data64=worseThanNormal; }
                else { data64=normal; }
                if (fslLpsAlign128Pct > 105.0) { data128=betterThanNormal; }
                else if (fslLpsAlign128Pct < 95.0) { data128=worseThanNormal; }
                else { data128=normal; }
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data64, fslLpsNoAlignPct);
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data64, fslLpsAlign64Pct);
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data128, fslLpsAlign128Pct);

            #cpu = (new / old) * 100.0
            printf("\t    <!-- FastSpinLock User+System -->\n");
                fslUTNoAlign=accum["FastSpinLock", "User", "NoAlign", i];
                fslUTAlign64=accum["FastSpinLock", "User", "Align64", i];
                fslUTAlign128=accum["FastSpinLock", "User", "Align128", i];
                fslSTNoAlign=accum["FastSpinLock", "System", "NoAlign", i];
                fslSTAlign64=accum["FastSpinLock", "System", "Align64", i];
                fslSTAlign128=accum["FastSpinLock", "System", "Align128", i];
                fslTimeNoAlign=fslUTNoAlign + fslSTNoAlign;
                fslTimeAlign64=fslUTAlign64 + fslSTAlign64;
                fslTimeAlign128=fslUTAlign128 + fslSTAlign128;

                fslTimeNoAlignPct=fslTimeNoAlign/mtxTimeNoAlign;
                fslTimeAlign64Pct=fslTimeAlign64/mtxTimeNoAlign;
                fslTimeAlign128Pct=fslTimeAlign128/mtxTimeNoAlign;
                fslTimeNoAlignPct *= 100.0;
                fslTimeAlign64Pct *= 100.0;
                fslTimeAlign128Pct *= 100.0;
                if (fslTimeNoAlignPct > 105.0) { data=worseThanNormal; }
                else if (fslTimeNoAlignPct < 95.0) { data=betterThanNormal; }
                else { data=normal; }
                if (fslTimeAlignPct > 105.0) { data64=worseThanNormal; }
                else if (fslTimeAlign64Pct < 95.0) { data64=betterThanNormal; }
                else { data64=normal; }
                if (fslTimeAlign128Pct > 105.0) { data128=worseThanNormal; }
                else if (fslTimeAlign128Pct < 95.0) { data128=betterThanNormal; }
                else { data128=normal; }
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data, fslTimeNoAlignPct);
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data64, fslTimeAlign64Pct);
                printf("\t        <td %s><span %s> %.3f</span></td>\n", align, data128, fslTimeAlign128Pct);

        printf("\t</tr>\n");
    }
}
