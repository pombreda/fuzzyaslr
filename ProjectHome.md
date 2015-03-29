/proc/pid/maps for processes not owned by you can no longer be read, but using some simple heuristics, it's possible to reconstruct the maps by sampling kstkeip.

kstkeip is a very low granularity counter, few processes worth attacking are compute bound for long periods of time. With the very few unique samples that can be acquired, it's not immediately obvious whether this information leak is useful or not (note that due to the way linkers typically use PT\_LOAD, data will be mapped immediately adjacent to text).

Fuzzy Aslr is a Proof of Concept implementation that attempts to "reconstruct" the address space layout of a process you don't own via kstkeip.

Thanks to x86 being a variable width architecture, by recursively disassembling the target binary and all DT\_NEEDED dependencies and recording the instruction boundaries it encounters, we're able to build up a fingerprint of each section of code that will be mapped.

Surprisingly, just a few (6 or so) samples of kstskeip from the same mapping is enough to uniquely fingerprint most code.

See ExampleUsage for more information.

Julien Tinnes and I described this technique and demonstrated this tool at CanSecWest 2009, [slides are available here](http://blog.cr0.org/2009/04/local-bypass-of-linux-aslr-through-proc.html).