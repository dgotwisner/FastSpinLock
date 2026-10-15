<table>
    <thead>
         <!-- Row 1 -->
         <tr>
             <th style"text-align:center; font-weight:bold;
>Threads</th>
             <th colspan="6" style"text-align:center; font-weight:bold;
>std::mutex</th>
             <th colspan="6" style"text-align:center; font-weight:bold;
>FastSpinLock</th>
         </tr>
         <!-- Row 2 -->
         <tr>
         <th></th>
             <th colspan="3" style"text-align:center; font-weight:bold;
>Locks/Second</th>
             <th colspan="3" style"text-align:center; font-weight:bold;
>User+System Time</th>
             <th colspan="3" style"text-align:center; font-weight:bold;
>Locks/Second</th>
             <th colspan="3" style"text-align:center; font-weight:bold;
>User+System Time</th>
         </tr>
         <!-- Row 3 -->
         <tr>
             <th></th>
             <th colspan="3" style"text-align:center;
>Locks/Second As a Percent<br/>(Greater Than 100% is best)</th>
             <th colspan="3" style"text-align:center;
>User+System Time As a Percent<br/>(Less Than 100% is best)</th>
             <th colspan="3" style"text-align:center;
>Locks/Second As a Percent<br/>(Greater Than 100% is best)</th>
             <th colspan="3" style"text-align:center;
>User+System Time As a Percent<br/>(Less Than 100% is best)</th>
         </tr>
         <!-- Row 4 -->
         <tr>
             <th></th>
             <th style"text-align:center; font-weight:bold;
>No align</th>
             <th style"text-align:center; font-weight:bold;
>Alignas(64)</th>
             <th style"text-align:center; font-weight:bold;
>Alignas(128)</th>
             <th style"text-align:center; font-weight:bold;
>No align</th>
             <th style"text-align:center; font-weight:bold;
>Alignas(64)</th>
             <th style"text-align:center; font-weight:bold;
>Alignas(128)</th>
             <th style"text-align:center; font-weight:bold;
>No align</th>
             <th style"text-align:center; font-weight:bold;
>Alignas(64)</th>
             <th style"text-align:center; font-weight:bold;
>Alignas(128)</th>
             <th style"text-align:center; font-weight:bold;
>No align</th>
             <th style"text-align:center; font-weight:bold;
>Alignas(64)</th>
             <th style"text-align:center; font-weight:bold;
>Alignas(128)</th>
         </tr>
         <!-- Row 5 -->
         <tr>
             <th></th>
             <th style"text-align:center;
></th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
></th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
>Percent</th>
             <th style"text-align:center;
>Percent</th>
         </tr>
     </thead>
     <tbody>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 0</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 35414052.507</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 34944135.825</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 35183426.950</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4.396</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4.454</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4.424</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 50239938.956</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 49975245.555</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 50622071.863</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3.569</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3.595</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3.550</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.673 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.349 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.319 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.637 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 141.864 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 141.117 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 142.943 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 81.187 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 81.779 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 80.755 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 6990012.858</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7219073.283</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7122395.009</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 37.619</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 37.552</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 37.598</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 11759434.207</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 10185062.208</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 9919177.389</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 26.474</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 28.894</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 29.146</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 103.277 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.894 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.822 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.944 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 168.232 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 145.709 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 141.905 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 70.374 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 76.807 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 77.477 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3272492.196</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3315878.576</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3305732.059</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 102.538</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 101.096</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 101.468</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7502863.296</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 6094786.489</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 6635457.825</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 59.608</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 70.790</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 67.049</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.326 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.016 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.594 ⚫</span></td>
	        <td style="text-align: right;"><span > 98.956 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 229.271 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 186.243 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 202.765 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 58.133 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 69.038 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 65.389 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1885010.299</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1933630.078</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1929676.695</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 225.943</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 220.752</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 220.968</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5164591.796</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5212193.756</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5125541.513</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 113.780</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 117.056</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 118.848</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 102.579 ⚫</span></td>
	        <td style="text-align: right;"><span > 102.370 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 97.703 ⚫</span></td>
	        <td style="text-align: right;"><span > 97.798 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 273.982 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 276.507 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 271.911 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 50.358 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 51.808 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 52.601 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1417964.998</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1421807.406</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1422307.155</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 366.041</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 365.604</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 364.856</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4458619.622</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4635251.434</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4651979.912</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 158.810</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 164.454</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 164.323</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.271 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.306 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.881 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.676 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 314.438 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 326.895 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 328.074 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 43.386 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 44.928 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 44.892 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1138799.643</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1151028.439</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1161514.601</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 537.703</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 532.131</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 527.655</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3939322.394</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3856724.154</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3863948.508</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 211.964</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 230.961</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 230.915</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.074 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.995 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.964 ⚫</span></td>
	        <td style="text-align: right;"><span > 98.131 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 345.919 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 338.666 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 339.300 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 39.420 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 42.953 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 42.945 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 6</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 945769.780</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 961709.199</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 961734.839</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 746.572</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 735.125</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 735.124</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3325069.884</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3235056.500</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3289890.089</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 290.690</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 316.306</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 315.232</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.685 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.688 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.467 ⚫</span></td>
	        <td style="text-align: right;"><span > 98.467 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 351.573 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 342.055 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 347.853 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 38.937 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 42.368 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 42.224 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 836535.678</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 836021.608</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 831744.370</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 961.314</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 961.557</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 967.904</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3013608.954</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2751836.943</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2755177.849</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 370.999</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 421.210</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 421.893</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.939 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.427 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.025 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.686 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 360.249 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 328.956 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 329.356 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 38.593 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 43.816 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 43.887 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 8</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 775009.779</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 762672.093</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 784770.516</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1166.861</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1186.760</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1152.523</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2625843.613</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2505477.630</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2489105.961</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 480.796</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 529.221</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 528.732</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.408 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.259 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.705 ⚫</span></td>
	        <td style="text-align: right;"><span > 98.771 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 338.814 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 323.283 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 321.171 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 41.204 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 45.354 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 45.312 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 9</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 723996.882</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 714079.891</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 725020.843</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1388.784</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1408.143</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1388.193</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2372237.816</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2237002.717</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2340544.187</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 595.976</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 653.086</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 639.715</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.630 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.141 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.394 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.957 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 327.659 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 308.980 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 323.281 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 42.914 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 47.026 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 46.063 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 10</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 656427.583</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 652673.602</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 655021.858</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1670.827</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1680.048</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1672.926</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1690339.496</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2041063.274</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2120844.285</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 810.949</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 793.098</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 775.193</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.428 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.786 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.552 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.126 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 257.506 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 310.935 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 323.089 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 48.536 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 47.467 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 46.396 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 11</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 603668.719</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 592589.289</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 604014.825</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1981.581</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2019.263</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1982.558</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1569975.837</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1895902.420</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1959041.348</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 957.713</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 938.878</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 924.718</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.165 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.057 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.902 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.049 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 260.072 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 314.063 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 324.523 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 48.331 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 47.380 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 46.666 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 12</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 536487.284</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 542691.694</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 552712.524</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2412.810</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2386.827</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2344.242</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1369233.273</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1589010.073</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1819588.394</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1181.085</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1102.722</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1014.447</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.156 ⚫</span></td>
	        <td style="text-align: right;"><span > 103.024 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.923 ⚫</span></td>
	        <td style="text-align: right;"><span > 97.158 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 255.222 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 296.188 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 339.167 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 48.951 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 45.703 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 42.044 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 13</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 496325.478</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 493013.089</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 501120.174</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2809.605</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2831.521</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2783.412</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1193316.741</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1391618.427</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1616809.236</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1432.677</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1274.402</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1153.948</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.333 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.966 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.780 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.068 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 240.430 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 280.384 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 325.756 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 50.992 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 45.359 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 41.072 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 14</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 467443.119</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 471536.603</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 464006.323</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3206.408</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3179.027</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3227.933</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1041810.636</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1233206.510</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1435322.688</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1748.546</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1473.892</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1318.177</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.876 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.265 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.146 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.671 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 222.874 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 263.820 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 307.058 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 54.533 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 45.967 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 41.111 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 15</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 440130.536</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 430036.674</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 430570.985</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3633.573</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3724.507</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 3709.711</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 970833.216</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1052552.120</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1198895.524</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2009.033</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1768.066</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1603.990</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 97.707 ⚫</span></td>
	        <td style="text-align: right;"><span > 97.828 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 102.503 ⚫</span></td>
	        <td style="text-align: right;"><span > 102.095 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 220.578 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 239.145 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 272.395 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 55.291 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 48.659 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 44.144 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 16</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 414139.710</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 417299.382</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 414620.827</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4100.720</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4071.807</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4096.465</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 861772.777</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 989857.108</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1098642.366</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2392.358</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1949.306</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 1774.823</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.763 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.116 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.295 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.896 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 208.087 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 239.015 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 265.283 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 58.340 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 47.536 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 43.281 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 17</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 392847.916</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 399327.832</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 397063.353</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4579.473</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4507.548</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 4536.793</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 752392.072</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 796039.924</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 988177.979</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2869.932</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2563.548</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2058.294</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.649 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.073 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.429 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.068 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 191.522 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 202.633 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 251.542 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 62.669 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 55.979 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 44.946 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 18</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 365887.530</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 355296.361</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 361234.553</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5172.732</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5323.793</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5244.065</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 399494.439</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 467282.336</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 795979.271</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5530.730</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5036.739</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 2615.915</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 97.105 ⚫</span></td>
	        <td style="text-align: right;"><span > 98.728 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 102.920 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.379 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 109.185 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 127.712 🟢</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 217.548 🟢</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 106.921 🔴</span></td>
	        <td style="text-align: right;"><span > 97.371 ⚫</span></td>
	        <td style="text-align: right;"><span style="color: rgb(0,255,0);"> 50.571 🟢</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 19</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 343351.266</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 339132.650</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 345012.454</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5812.136</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5873.528</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 5777.743</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 243713.205</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 233500.182</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 302519.050</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 8515.741</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 8738.884</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 6843.827</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.771 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.484 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.056 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.408 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 70.981 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 68.006 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 88.108 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 146.517 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 150.356 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 117.751 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 20</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 324448.738</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 325592.373</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 331685.659</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 6450.764</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 6424.408</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 6308.247</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 178067.169</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 171432.868</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 195419.298</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 12022.858</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 12414.315</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 10884.229</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.352 ⚫</span></td>
	        <td style="text-align: right;"><span > 102.231 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.591 ⚫</span></td>
	        <td style="text-align: right;"><span > 97.791 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 54.883 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 52.838 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 60.231 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 186.379 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 192.447 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 168.728 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 21</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 312572.668</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 310245.070</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 311547.506</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7011.467</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7051.405</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7038.458</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 152096.561</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 140699.656</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 151209.127</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 14738.142</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 15771.588</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 14653.400</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.255 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.672 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.570 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.385 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 48.660 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 45.013 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 48.376 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 210.201 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 224.940 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 208.992 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 22</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 302066.350</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 298716.522</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 302760.342</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7586.841</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7678.768</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 7571.574</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 158692.741</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 150996.955</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 159257.686</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 14793.030</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 15378.472</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 14575.306</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.891 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.230 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.212 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.799 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 52.536 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 49.988 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 52.723 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 194.983 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 202.699 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 192.113 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 23</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 286355.688</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 287899.943</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 280891.603</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 8347.043</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 8307.219</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 8517.369</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 115680.672</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 110601.514</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 112951.623</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 21079.547</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 21814.093</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 21391.694</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.539 ⚫</span></td>
	        <td style="text-align: right;"><span > 98.092 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.523 ⚫</span></td>
	        <td style="text-align: right;"><span > 102.041 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 40.398 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 38.624 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 39.445 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 252.539 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 261.339 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 256.279 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 24</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 276649.441</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 271166.609</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 274828.361</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 9002.962</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 9189.583</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 9066.322</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 126209.119</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 118392.849</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 119593.461</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 20201.617</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 21207.217</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 21037.566</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.018 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.342 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 102.073 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.704 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 45.621 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 42.795 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 43.229 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 224.389 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 235.558 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 233.674 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 25</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 266688.795</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 263970.190</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 261646.284</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 9719.040</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 9812.419</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 9903.635</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 98653.874</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 89630.032</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 89092.723</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 26817.789</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 29102.813</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 29289.250</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.981 ⚫</span></td>
	        <td style="text-align: right;"><span > 98.109 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.961 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.899 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 36.992 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 33.608 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 33.407 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 275.930 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 299.441 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 301.359 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 26</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 256041.460</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 251502.369</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 256703.285</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 10489.049</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 10675.836</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 10463.097</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 107740.816</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 96198.417</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 99628.339</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 25221.200</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 28146.670</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 27226.012</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 98.227 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.258 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 101.781 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.753 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 42.079 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 37.571 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 38.911 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 240.453 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 268.343 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 259.566 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 27</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 241578.440</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 242582.087</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 240120.272</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 11533.465</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 11481.770</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 11599.614</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 83281.079</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 77487.484</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 75825.521</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 33711.036</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 36238.591</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 37016.156</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.415 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.396 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.552 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.574 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 34.474 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 32.075 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 31.388 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 292.289 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 314.204 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 320.946 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 28</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 231696.014</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 231753.529</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 235399.801</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 12452.959</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 12454.059</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 12250.922</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 92065.063</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 85634.278</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 81121.257</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 31689.874</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 34006.420</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 35856.360</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.025 ⚫</span></td>
	        <td style="text-align: right;"><span > 101.599 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.009 ⚫</span></td>
	        <td style="text-align: right;"><span > 98.378 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 39.735 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 36.960 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 35.012 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 254.477 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 273.079 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 287.934 🔴</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 29</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 223745.574</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 222607.111</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 225560.779</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 13342.744</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 13407.378</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 13231.294</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 73367.133</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 66676.362</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 68334.204</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="font-weight:bold;"> 41130.458</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 45078.236</span></td>
	        <td style="text-align: right;"><span style="font-weight:bold;"> 43970.624</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align: right;"><span >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 99.491 ⚫</span></td>
	        <td style="text-align: right;"><span > 100.811 ⚫</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align: right;"><span ></span></td>
	        <td style="text-align: right;"><span > 100.484 ⚫</span></td>
	        <td style="text-align: right;"><span > 99.165 ⚫</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 32.790 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 29.800 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 30.541 🔴</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 308.261 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 337.848 🔴</span></td>
	        <td style="text-align: right;"><span style="color: rgb(255,0,0);"> 329.547 🔴</span></td>
	</tr>
    </tbody>
</table>
