# VeeamID
Small tool to extract an ID from veeamconfig utility. Pipe output to it to select the first match

```
veeamconfig backup list | veeamid
```

Use -c to count the number of matches
```
BACKUPID=$(veeamconfig backup list | veeamid)
veeamconfig point list --backupId $BACKUPID | veeamid -c
```

Use -n<number> to select <number output>. For example to select the second match, use n2
```
BACKUPID=$(veeamconfig backup list | veeamid)
veeamconfig point list --backupId $BACKUPID | veeamid -n2
```

The matching is based on regex:
```
[{]([a-z0-9-]+)[}]
```
So it matches everything in lower case, numbers and '-' between curly brackets e.g {ab-1} would also match

Compiling can be done with:
```
compile with gcc veeamid.c -o veeamid
```

![Example](veeamid.png)

# Distributed under MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
