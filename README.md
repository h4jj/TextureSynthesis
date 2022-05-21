# Texture Synthesis - Based off [TSVQ by Wei and Levoy](https://graphics.stanford.edu/papers/texture-synthesis-sig00/texture.pdf)

The main function currently has 3 methods running if you wish to run each variation 
separately (single , multi, multi + TSVQ) simply comment out the methods and leave the desired
one uncommented. 

```bash
# SingleResolutionSynthesis()
# MultiResolutionSynthesis
# MultiResolutionSynthesisTSVQ()
```

In order to change the neighborhood sizes, at the top of each of the methods
is an integer value "const int nbGrid = N". Changing N into your desired value will construct an
NxN neighborhood, everything else is setup perfectly.

Running the current implementation will run all three variations with a 9x9 neighborhood for the 
single resolution synthesis and a 2 level 5x5 neighborhood for the multi resolution case
