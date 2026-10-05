#!/bin/bash

build() {
    echo Building...
    for i in {0..29}
    do
        echo building $i mutex\:
        g++ -O3 --std=c++11 -DTHREADCOUNT=$i jibbrish.cpp -o jibbrish-mtx-${i}.exe -lpthread &
        echo building $i mutex timing\:
        g++ -O3 --std=c++11 -DTIMING=1 -DTHREADCOUNT=$i jibbrish.cpp -o jibbrish-mtx-timing-${i}.exe -lpthread &
        echo building $i mutex rdtscp\:
        g++ -O3 --std=c++11 -DRDTSCP=1 -DTHREADCOUNT=$i jibbrish.cpp -o jibbrish-mtx-rdtscp-${i}.exe -lpthread &
        echo building $i mutex rdtsc\:
        g++ -O3 --std=c++11 -DRDTSC=1 -DTHREADCOUNT=$i jibbrish.cpp -o jibbrish-mtx-rdtsc-${i}.exe -lpthread &

        echo building $i mythread\:
        g++ -O3 --std=c++11 -DMYLOCK=1 -DTHREADCOUNT=$i jibbrish.cpp -o jibbrish-mylock-${i}.exe -lpthread &
        echo building $i mythread timing\:
        g++ -O3 --std=c++11 -DMYLOCK=1 -DTIMING=1 -DTHREADCOUNT=$i jibbrish.cpp -o jibbrish-mylock-timing-${i}.exe -lpthread &
        echo building $i mythread rdtscp\:
        g++ -O3 --std=c++11 -DMYLOCK=1 -DRDTSCP=1 -DTHREADCOUNT=$i jibbrish.cpp -o jibbrish-mylock-rdtscp-${i}.exe -lpthread &
        echo building $i mythread rdtsc\:
        g++ -O3 --std=c++11 -DMYLOCK=1 -DRDTSC=1 -DTHREADCOUNT=$i jibbrish.cpp -o jibbrish-mylock-rdtsc-${i}.exe -lpthread &
        wait
    done
}

run() {
    echo Running...
    rm -f results.txt
    for i in {0..29}
    do
        for j in {1..10}
        do
            echo "Pass $j (mtx)"
            ./jibbrish-mtx-${i}.exe 1 $i | tee -a results.txt
            echo "Pass $j (mylock)"
            ./jibbrish-mylock-${i}.exe 2 $i | tee -a results.txt
        done
    done
}

time build
time run
awk -f results.gawk results.txt
