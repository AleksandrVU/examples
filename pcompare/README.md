The pcompare utility loads two branches package information in JSON file and compare them.
It output comparison statistic in JSON format that includes 3 arrays:
 1. Which packages is absent in first branch
 2. Which packages is absent in second branch
 3. All packages in first branch with newer version then in second one

The utility requires installed 3 libs:
1. libpthread
2. lincurl
3. libjson-c

Usage:
 pcompare p9 p10

