


#define OLD_ALPHA_BLEND
//NOTE: OLD_ALPHA_BLEND blends from source screen to target screen. This words better for MRI images. This only works
//      when a single face is displayed. It does not if more than one face of the source cube is visible.


//#define NEW_ALPHA_BLEND
//NOTE: NEW_ALPHA_BLEND blends from target volume to source screen. For this works good for ultrasound images
//      but not so good for MRI images. This will work if the source cube has more than one cube face visible.