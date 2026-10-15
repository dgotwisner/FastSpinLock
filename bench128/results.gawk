BEGIN {
        inrecord = 0;
        prog=""
        type=""
        threads=""
        real=0
        user=0
        sys=0
        locksPerSec=0.0
	iter=0
	seconds=0
	nsec=0

        printf("#Prog type threads real user sys cumIterations cumSeconds locks/sec\n") > "stdMutex.out"
        printf("#Prog type threads real user sys cumIterations cumSeconds locks/sec\n") > "myMutex.out"
}
{
        if (/^BEGIN TEST/) {
            inrecord = 1
        } else if (/^Running /) {
            prog=$2
            type=$4
            threads=$6
        } else if (/^.*StdMutex/) {
            locksPerSec = $5
        } else if (/^.*MyLock/) {
            locksPerSec = $5
        } else if (/^[ \t]main/) {
	    iter+=$2
	    seconds+=$4
	    nsec+=$6
        } else if (/^[ \t]thread/) {
	    iter+=$3
	    seconds+=$5
	    nsec+=$7
        } else if (/^real/) {
            real=$2
        } else if (/^user/) {
            user=$2
        } else if (/^sys/) {
            sys=$2
	    seconds += (nsec / 1000000000)
            nsec /= 1000000000
            # Now factor in the fractional nsec:
            seconds *= 1000000000
            seconds += nsec
            seconds /= 1000000000

            # And output
            if (type == 1) {
                printf("%s %d %d %f %f %f %d %f %f\n",
                    prog, type, threads, real, user, sys, iter, seconds, locksPerSec) >> "stdMutex.out"
            } else if (type == 2) {
                printf("%s %d %d %f %f %f %d %f %f\n",
                    prog, type, threads, real, user, sys, iter, seconds, locksPerSec) >> "myMutex.out"
            } else {
                print "Unexpected type " type "\n"
            }
	    inrecord = 0
	    prog=""
	    type=""
	    threads=""
	    real=0
	    user=0
	    sys=0
	    locksPerSec=0.0
	    iter=0
	    seconds=0
	    nsec=0
        }
}
END {
}
