# Diskspeed

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/7e95d422a4cc49f2b5ed131e81483cc9)](https://app.codacy.com/manual/Dko1905/diskspeed?utm_source=github.com&utm_medium=referral&utm_content=Dko1905/diskspeed&utm_campaign=Badge_Grade_Dashboard)

A utility written in c to test read &amp; write speed on disks.
It uses gnu11 for `clock_gettime` and other things.

## Problems
The program read and writes are not accurate because of caching in linux.

## License
MIT
