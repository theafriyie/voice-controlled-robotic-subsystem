/* Voice-Controlled Robotic Subsystem
 * Board: Arduino Nano 33 BLE Sense
 * ML Tool: Edge Impulse
 * Motor: 28BYJ-48 Stepper Motor
 * Driver: ULN2003
 *
 * Description:
 * This system listens for voice commands using the onboard microphone.
 * When "BACKWARD" is detected, the motor rotates clockwise.
 * When "FORWARD" is detected, the motor rotates counterclockwise.
 */

#define EIDSP_QUANTIZE_FILTERBANK 0

#include <PDM.h>
#include <Robotic_SubSystem_inferencing.h>
#include <Stepper.h>


// Stepper motor configuration-
const int stepsPerRevolution = 2048;

// Pins entered in sequence IN1-IN3-IN2-IN4
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);


// Audio inference buffers
typedef struct {
    int16_t *buffer;
    uint8_t buf_ready;
    uint32_t buf_count;
    uint32_t n_samples;
} inference_t;

static inference_t inference;
static signed short sampleBuffer[2048];
static bool debug_nn = false;


// Function declarations
static bool microphone_inference_start(uint32_t n_samples);
static bool microphone_inference_record(void);
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr);
static void microphone_inference_end(void);
static void pdm_data_ready_inference_callback(void);

// Setup
void setup()
{
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Edge Impulse Inferencing Demo");

    // Summary of inferencing settings
    ei_printf("Inferencing settings:\n");
    ei_printf("\tInterval: %.2f ms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
    ei_printf("\tNo. of classes: %d\n",
              sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

    if (microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false) {
        ei_printf("ERR: Could not allocate audio buffer (size %d), this could be due to the window length of your model\r\n",
                  EI_CLASSIFIER_RAW_SAMPLE_COUNT);
        return;
    }

    myStepper.setSpeed(10);  // 10 RPM
}

// Main loop
void loop()
{
    ei_printf("Starting inferencing in 2 seconds...\n");
    delay(2000);

    ei_printf("Recording...\n");

    bool m = microphone_inference_record();
    if (!m) {
        ei_printf("ERR: Failed to record audio...\n");
        return;
    }

    ei_printf("Recording done\n");

    signal_t signal;
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
    signal.get_data = &microphone_audio_signal_get_data;

    ei_impulse_result_t result = {0};

    EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debug_nn);
    if (r != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", r);
        return;
    }

    // Print prediction timing
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): ",
              result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf("\n");

    // Print classification scores
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: %.5f\n",
                  result.classification[ix].label,
                  result.classification[ix].value);
    }

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif

 

    // classification[0] = BACKWARD
    // classification[1] = FORWARD

    if (result.classification[0].value > 0.9) {
        ei_printf("BACKWARD");

        // Rotate clockwise at 10 RPM
        myStepper.setSpeed(10);
        myStepper.step(stepsPerRevolution);
        delay(1000);
    }
    else if (result.classification[1].value > 0.9) {
        ei_printf("FORWARD");

        // Rotate counterclockwise at 10 RPM
        myStepper.setSpeed(10);
        myStepper.step(-stepsPerRevolution);
        delay(1000);
    }
}

// PDM callback
static void pdm_data_ready_inference_callback(void)
{
    int bytesAvailable = PDM.available();
    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    if (inference.buf_ready == 0) {
        for (int i = 0; i < bytesRead / 2; i++) {
            inference.buffer[inference.buf_count++] = sampleBuffer[i];

            if (inference.buf_count >= inference.n_samples) {
                inference.buf_count = 0;
                inference.buf_ready = 1;
                break;
            }
        }
    }
}

// Start microphone inference
static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffer = (int16_t *)malloc(n_samples * sizeof(int16_t));

    if (inference.buffer == NULL) {
        return false;
    }

    inference.buf_count = 0;
    inference.n_samples = n_samples;
    inference.buf_ready = 0;

    // Configure the data receive callback
    PDM.onReceive(&pdm_data_ready_inference_callback);
    PDM.setBufferSize(4096);

  
    if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
        ei_printf("Failed to start PDM!");
        microphone_inference_end();
        return false;
    }

    // Set gain
    PDM.setGain(127);

    return true;
}

// Wait for new audio data
static bool microphone_inference_record(void)
{
    inference.buf_ready = 0;
    inference.buf_count = 0;

    while (inference.buf_ready == 0) {
        delay(10);
    }

    return true;
}

// Get raw audio signal data
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffer[offset], out_ptr, length);
    return 0;
}

// Stop PDM and release buffers
static void microphone_inference_end(void)
{
    PDM.end();
    free(inference.buffer);
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_MICROPHONE
#error "Invalid model for current sensor."
#endif
