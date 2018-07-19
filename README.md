# MTV Rhythm Generator

![App Screenshot](App/Screenshots/MtvRhythmGeneratorCapture_68ce27.PNG?raw=true "MTV Rhythm Generator App Screenshot")
App demo: [YouTube][2]

This app demonstrates the [Metrical Tension Vector][1] (MTV) representation of monophonic rhythms. These vectors live in N-dimensional vector spaces, where N is the length of the rhythms as a multiple of the fastest time unit. The axes of this space represent the degree of metrical tension (or syncopation) on the corresponding axis' position in time.

The main canvas of the app (gray) shows two lines, a blue and a gray one. Both lines represent a point in the current MTV space (dependent on time signature and step size). The gray line shows the MTV of the current rhythm pattern (in the sequencer). The blue line is drawable and can be used to create a target point in the MTV space. By pressing G, the app generates a rhythm whose point in MTV space is close to the target point (A normally-distributed random distance is chosen and quantized to the nearest pattern. Then, of all patterns sharing that distance (usually not more than 3 of 4), a random pattern is returned). The distance between the current rhythm's point and the target point is displayed in the upper right corner of the canvas, normalized from 0 to 1.

[1]: https://tomasito665.github.io/beatsearch/reference.html#beatsearch.feature_extraction.MonophonicMetricalTensionVector
[2]: https://www.youtube.com/watch?v=xb5obNrhkuA
