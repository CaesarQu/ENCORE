# SPIDER3D
repository for the SPIDER3D algorithm.

We introduce a novel environmentally-aware cooperative drone path computation scheme viz., **SPIDER-3D** that can serve needs of full autonomy in multi-drone, multi-truck last-mile parcel delivery tasks. 
First, our 3D \textit{offline} approach builds upon the geographic routing scheme in SPIDER, and involves proactively ensuring energy efficiency, while maximizing the throughput of parcel delivery. Our SPIDER-3D approach that is inherently offline-based, provides resilience to FANETs that are relevant to the delivery application scenario by overcoming barriers due to obstacles that impact A2A and A2G network communications. The resilience feature in SPIDER-3D geographic routing is due to our ability to ensure that the reconstruction of the A2A or A2G network communications has limited overhead on the Oracle delivery task, which is attributed to the state-of-the-art *parcel delivery scheduling algorithm*.

## Apply SPIDER-3D in parcel delivery procedure

**SPIDER-3D** is first used inside parcel delivery applications. To apply SPIDER-3D, a scheduling algorithm result needs to be given. For instance, in our use case, we use [mFSTSP algorithm](https://github.com/optimatorlab/mFSTSP). We considered 1-4 drones and 0-2 trucks on the parcel delivery scenario. 

## NS-3 scripts

Folder [ns-3_sim](./ns-3_sim) provides SPIDER3D implementation scripts. SPIDER3D needs to have location-based module in NS-3 as a add-on, which is also provided in the folder.

## Preliminary results:

|      | Drone | GEAR (with $\theta$ = 0.75) |      1     | SPIDER-3D (with $\theta$ = 0) |    0.25    |     0.5    |    0.75    |      1     |    AODV    |    HWMP    |
|:----:|:-----:|:---------------------------:|:----------:|:-----------------------------:|:----------:|:----------:|:----------:|:----------:|:----------:|:----------:|
|  low |   1   |          525$\pm$70         | 522$\pm$46 |           519$\pm$34          | 520$\pm$37 | 519$\pm$42 | 522$\pm$37 | 520$\pm$31 | 489$\pm$27 | 563$\pm$61 |
|      |   3   |          514$\pm$14         | 515$\pm$13 |           514$\pm$12          | 507$\pm$18 | 506$\pm$18 | 506$\pm$50 | 501$\pm$40 | 480$\pm$31 | 532$\pm$89 |
| high |   1   |          502$\pm$52         | 497$\pm$47 |           503$\pm$49          | 498$\pm$36 | 498$\pm$40 | 500$\pm$39 | 501$\pm$40 | 445$\pm$25 | 555$\pm$52 |
|      |   3   |          510$\pm$13         | 499$\pm$16 |           504$\pm$48          | 504$\pm$46 | 495$\pm$40 | 493$\pm$39 | 493$\pm$39 | 465$\pm$26 | 531$\pm$86 |


## Future Plan

Provide security strategies inside SPIDER3D, add pre-existing packages on NS-3 to support. Experiments with attack dataset on IoT devices/sensors are considered.