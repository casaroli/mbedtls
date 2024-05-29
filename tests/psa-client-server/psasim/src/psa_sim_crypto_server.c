/* THIS FILE WAS AUTO-GENERATED BY psa_sim_generate.pl. DO NOT EDIT!! */

/* server implementations */

#include <stdio.h>
#include <stdlib.h>

#include <psa/crypto.h>

#include "psa_functions_codes.h"
#include "psa_sim_serialise.h"

#include "service.h"

// Returns 1 for success, 0 for failure
int psa_crypto_init_wrapper(
    uint8_t *in_params, size_t in_params_len,
    uint8_t **out_params, size_t *out_params_len)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    uint8_t *result = NULL;
    int ok;

    // Now we call the actual target function

    status = psa_crypto_init(
        );

    // NOTE: Should really check there is no overflow as we go along.
    size_t result_size =
        psasim_serialise_begin_needs() +
        psasim_serialise_psa_status_t_needs(status);

    result = malloc(result_size);
    if (result == NULL) {
        goto fail;
    }

    uint8_t *rpos = result;
    size_t rremain = result_size;

    ok = psasim_serialise_begin(&rpos, &rremain);
    if (!ok) {
        goto fail;
    }

    ok = psasim_serialise_psa_status_t(&rpos, &rremain, status);
    if (!ok) {
        goto fail;
    }

    *out_params = result;
    *out_params_len = result_size;

    return 1;   // success

fail:
    free(result);
    return 0;       // This shouldn't happen!
}

// Returns 1 for success, 0 for failure
int psa_hash_compute_wrapper(
    uint8_t *in_params, size_t in_params_len,
    uint8_t **out_params, size_t *out_params_len)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_algorithm_t alg;
    uint8_t *input = NULL;
    size_t input_length;
    uint8_t *hash = NULL;
    size_t hash_size;
    size_t hash_length;

    uint8_t *pos = in_params;
    size_t remaining = in_params_len;
    uint8_t *result = NULL;
    int ok;

    ok = psasim_deserialise_begin(&pos, &remaining);
    if (!ok) {
        goto fail;
    }

    ok = psasim_deserialise_psa_algorithm_t(&pos, &remaining, &alg);
    if (!ok) {
        goto fail;
    }

    ok = psasim_deserialise_buffer(&pos, &remaining, &input, &input_length);
    if (!ok) {
        goto fail;
    }

    ok = psasim_deserialise_buffer(&pos, &remaining, &hash, &hash_size);
    if (!ok) {
        goto fail;
    }

    ok = psasim_deserialise_size_t(&pos, &remaining, &hash_length);
    if (!ok) {
        goto fail;
    }

    // Now we call the actual target function

    status = psa_hash_compute(
        alg,
        input, input_length,
        hash, hash_size,
        &hash_length
        );

    // NOTE: Should really check there is no overflow as we go along.
    size_t result_size =
        psasim_serialise_begin_needs() +
        psasim_serialise_psa_status_t_needs(status) +
        psasim_serialise_buffer_needs(hash, hash_size) +
        psasim_serialise_size_t_needs(hash_length);

    result = malloc(result_size);
    if (result == NULL) {
        goto fail;
    }

    uint8_t *rpos = result;
    size_t rremain = result_size;

    ok = psasim_serialise_begin(&rpos, &rremain);
    if (!ok) {
        goto fail;
    }

    ok = psasim_serialise_psa_status_t(&rpos, &rremain, status);
    if (!ok) {
        goto fail;
    }

    ok = psasim_serialise_buffer(&rpos, &rremain, hash, hash_size);
    if (!ok) {
        goto fail;
    }

    ok = psasim_serialise_size_t(&rpos, &rremain, hash_length);
    if (!ok) {
        goto fail;
    }

    *out_params = result;
    *out_params_len = result_size;

    return 1;   // success

fail:
    free(result);
    return 0;       // This shouldn't happen!
}

psa_status_t psa_crypto_call(psa_msg_t msg)
{
    int ok = 0;

    int func = msg.type;

    /* We only expect a single input buffer, with everything serialised in it */
    if (msg.in_size[1] != 0 || msg.in_size[2] != 0 || msg.in_size[3] != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* We expect exactly 2 output buffers, one for size, the other for data */
    if (msg.out_size[0] != sizeof(size_t) || msg.out_size[1] == 0 ||
        msg.out_size[2] != 0 || msg.out_size[3] != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t *in_params = NULL;
    size_t in_params_len = 0;
    uint8_t *out_params = NULL;
    size_t out_params_len = 0;

    in_params_len = msg.in_size[0];
    in_params = malloc(in_params_len);
    if (in_params == NULL) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    /* Read the bytes from the client */
    size_t actual = psa_read(msg.handle, 0, in_params, in_params_len);
    if (actual != in_params_len) {
        free(in_params);
        return PSA_ERROR_CORRUPTION_DETECTED;
    }

    switch (func) {
        case PSA_CRYPTO_INIT:
            ok = psa_crypto_init_wrapper(in_params, in_params_len,
                                         &out_params, &out_params_len);
            break;
        case PSA_HASH_COMPUTE:
            ok = psa_hash_compute_wrapper(in_params, in_params_len,
                                          &out_params, &out_params_len);
            break;
    }

    free(in_params);

    if (out_params_len > msg.out_size[1]) {
        fprintf(stderr, "unable to write %zu bytes into buffer of %zu bytes\n",
                out_params_len, msg.out_size[1]);
        exit(1);
    }

    /* Write the exact amount of data we're returning */
    psa_write(msg.handle, 0, &out_params_len, sizeof(out_params_len));

    /* And write the data itself */
    if (out_params_len) {
        psa_write(msg.handle, 1, out_params, out_params_len);
    }

    free(out_params);

    return ok ? PSA_SUCCESS : PSA_ERROR_GENERIC_ERROR;
}
