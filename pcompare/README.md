The libpcompare library able to load information from two packages' branches in JSON format and compare them.
It outputs comparison statistic in JSON format that includes 3 arrays:
 1. Which packages is absent in first branch
 2. Which packages is absent in second branch
 3. All packages in first branch with newer version then in second one

The library has dependency from 4 libraries:
1. libpthread
2. libcurl
3. libjson-c
4. librmevercmp

Librmevercmp library included here as well. It is built from source code that has been taken from the RPM package manager.

Utility "ucompare" uses libpcompare to load packages information from two branches
Usage:
 ucompare p9 p10

