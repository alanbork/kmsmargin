// gcc -o margin margin.c -ldrm -lgbm -lEGL -lGLESv2 -I/usr/include/libdrm -I/usr/include/GLES2
// set overscan margins for pi4 or any pi running kms (also fkms with code change)

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


// The following code related to DRM/GBM was adapted from the following sources:
// https://gitlab.freedesktop.org/mesa/drm/-/blob/master/tests/proptest/proptest.c
// https://alantechreview.blogspot.com/2021/04/setting-custom-video-mode-using-drmkms.html
// and
// https://github.com/eyelash/tutorials/blob/master/drm-gbm.c
// and
// https://www.raspberrypi.org/forums/viewtopic.php?t=243707#p1499181
// and
// https://gitlab.freedesktop.org/mesa/drm/-/blob/master/tests/modetest/modetest.c
// and
// https://docs.huihoo.com/doxygen/linux/kernel/3.7/drm__modes_8c_source.html


int device;
static drmModeModeInfo mode_info; // our local copy so we can free resources
drmModeModeInfo *mode; // most functions want a pointer
uint32_t connectorId;
static const char *eglGetErrorStr(); // moved to bottom

static drmModeConnector* getConnector(drmModeRes* resources)
    {
    for (int i = 0; i < resources->count_connectors; i++)
        {
        drmModeConnector* connector = drmModeGetConnector(device, resources->connectors[i]);
        if (connector->connection == DRM_MODE_CONNECTED)
            {
            return connector;
            }
        drmModeFreeConnector(connector);
        }

    return NULL;
    }

static drmModeEncoder* findEncoder(drmModeConnector* connector)
    {
    if (connector->encoder_id)
        {
        return drmModeGetEncoder(device, connector->encoder_id);
        }
    return NULL;
    }


int main(int argc, char* argv[])
{
EGLDisplay display;
drmModeRes* resources;

int top, right, bottom, left; // https://developer.mozilla.org/en-US/docs/Web/CSS/margin

if (argc==1)
  {
  printf("Set overscan margin for KMS/DRM video driver. Max inset is 100. Syntax:\n"
  "<top> [right] [bottom] [left], unspecified margins inherit correspondingly  ala CSS margins\n");
  exit(0);
  }

if (argc>1)
  {
  top = right = bottom = left = atoi(argv[1]);
  }

if (argc>2)
  {
  right = left = atoi(argv[2]);
  }

  if (argc>3)
  {
  bottom = atoi(argv[3]);
  }
  
  if (argc>4)
  {
  left = atoi(argv[4]);
  }

// we have to try card0 and card1 to see which is valid. fopen will work on both, so...
device = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);

if ((resources = drmModeGetResources(device)) == NULL) // if we have the right device we can get it's resources
    {
    printf("/dev/dri/card0 does not have DRM resources, using card1\n");
    device = open("/dev/dri/card1", O_RDWR | O_CLOEXEC); // if not, try the other one: (1)
    resources = drmModeGetResources(device);
    }
else
    printf("using /dev/dri/card0\n");

if (resources == NULL)
    {
    printf("Unable to get DRM resources on card1\n"); return -1;
    }

drmModeConnector* connector = getConnector(resources);
if (connector == NULL)
    {
    fprintf(stderr, "Unable to get connector\n");
    drmModeFreeResources(resources);
    return -1;
    }

connectorId = connector->connector_id;
/*
// fkms values
drmModeObjectSetProperty(device, connectorId, DRM_MODE_OBJECT_CONNECTOR,           90, left);
drmModeObjectSetProperty(device, connectorId, DRM_MODE_OBJECT_CONNECTOR,           91, right);
drmModeObjectSetProperty(device, connectorId, DRM_MODE_OBJECT_CONNECTOR,           92, top);
drmModeObjectSetProperty(device, connectorId, DRM_MODE_OBJECT_CONNECTOR,           93, bottom);
  */
  
//kms values:
drmModeObjectSetProperty(device, connectorId, DRM_MODE_OBJECT_CONNECTOR,           33, left);
drmModeObjectSetProperty(device, connectorId, DRM_MODE_OBJECT_CONNECTOR,           34, right);
drmModeObjectSetProperty(device, connectorId, DRM_MODE_OBJECT_CONNECTOR,           35, top);
drmModeObjectSetProperty(device, connectorId, DRM_MODE_OBJECT_CONNECTOR,           36, bottom);

//printf("%i", connectorId); // note that kms has more than one connector, should handle that properly

close(device);
return EXIT_SUCCESS;
}

/* if the IDS are actually dynamic, we could match by name based on this code:
dump_prop(uint32_t prop_id, uint64_t value)
{
	int i;
	drmModePropertyPtr prop;

	prop = drmModeGetProperty(fd, prop_id);

	printf("\t%d", prop_id);
	if (!prop) {
		printf("\n");
		return;
	}

	printf(" %s:\n", prop->name); */
	


// Get the EGL error back as a string. Useful for debugging.
static const char *eglGetErrorStr()
{
    switch (eglGetError())
    {
    case EGL_SUCCESS:
        return "The last function succeeded without error.";
    case EGL_NOT_INITIALIZED:
        return "EGL is not initialized, or could not be initialized, for the "
               "specified EGL display connection.";
    case EGL_BAD_ACCESS:
        return "EGL cannot access a requested resource (for example a context "
               "is bound in another thread).";
    case EGL_BAD_ALLOC:
        return "EGL failed to allocate resources for the requested operation.";
    case EGL_BAD_ATTRIBUTE:
        return "An unrecognized attribute or attribute value was passed in the "
               "attribute list.";
    case EGL_BAD_CONTEXT:
        return "An EGLContext argument does not name a valid EGL rendering "
               "context.";
    case EGL_BAD_CONFIG:
        return "An EGLConfig argument does not name a valid EGL frame buffer "
               "configuration.";
    case EGL_BAD_CURRENT_SURFACE:
        return "The current surface of the calling thread is a window, pixel "
               "buffer or pixmap that is no longer valid.";
    case EGL_BAD_DISPLAY:
        return "An EGLDisplay argument does not name a valid EGL display "
               "connection.";
    case EGL_BAD_SURFACE:
        return "An EGLSurface argument does not name a valid surface (window, "
               "pixel buffer or pixmap) configured for GL rendering.";
    case EGL_BAD_MATCH:
        return "Arguments are inconsistent (for example, a valid context "
               "requires buffers not supplied by a valid surface).";
    case EGL_BAD_PARAMETER:
        return "One or more argument values are invalid.";
    case EGL_BAD_NATIVE_PIXMAP:
        return "A NativePixmapType argument does not refer to a valid native "
               "pixmap.";
    case EGL_BAD_NATIVE_WINDOW:
        return "A NativeWindowType argument does not refer to a valid native "
               "window.";
    case EGL_CONTEXT_LOST:
        return "A power management event has occurred. The application must "
               "destroy all contexts and reinitialise OpenGL ES state and "
               "objects to continue rendering.";
    default:
        break;
    }
    return "Unknown error!";
}
