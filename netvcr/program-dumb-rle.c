#include <stdint.h>
#include <string.h>

const uint8_t dumb_rle_file[] =
    #include "hdmi_phy_test1_wrapper.bin.rle.h"
;

struct dumb_rle_state {
    const uint8_t *data;
    int size;

    const uint8_t *read_offset;
    int rle_state;      // 0 = zeroes, 1 = data
    int rle_bytes_left; // Number of bytes left in this state
    int bytes_left;     // Number of bytes left until EOF.
};

static void dumb_rle_get_state(struct dumb_rle_state *state) {
    uint16_t type;

    memcpy(&type, state->read_offset, sizeof(type));

    state->read_offset += sizeof(type);
    state->bytes_left -= sizeof(type);

    /* If the high bit is set, then it's a run of zeroes. */
    if (type & 0x8000) {
        type &= ~0x8000;
        state->rle_state = 0;
    }
    state->rle_bytes_left = type;
}

void dumbRleInit(struct dumb_rle_state *state, const uint8_t *data, int size) {

    memset(state, 0, sizeof(*state));
    state->data = data;
    state->size = size;

    state->read_offset = state->data;
    state->bytes_left = state->size;

    dumb_rle_get_state(state);
}

void programDumbRleFile(void) {
    struct dumb_rle_state state;

    dumbRleInit(&state, dumb_rle_file, sizeof(dumb_rle_file));

    /*
    Update SPI ROM:
        1. MCU disconnects FPGA by de-asserting FPGA_DRIVE
        2. MCU configures drive on SPI pins
        3. Data arrives via USB and is written into SPI
        4. MCU tri-states its SPI pins
        5. MCU re-connects FPGA by asserting FPGA_DRIVE
        6. (optional) MCU initiates FPGA configuration by yanking PROGRAM
    */
}
