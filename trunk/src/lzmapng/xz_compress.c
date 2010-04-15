// Copyright 2010 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>

#include "lzma.h"

static const size_t kBufSize = 4096;

int Compress(FILE* in, FILE* out, int compress) {
  char inbuf[kBufSize];
  char outbuf[kBufSize];
  lzma_stream lstream = LZMA_STREAM_INIT;
  lzma_ret rv;
  lzma_options_lzma options;
  lzma_lzma_preset(&options, LZMA_PRESET_DEFAULT);
  if (compress) {
    /*
    rv = lzma_easy_encoder(&lstream,
                           LZMA_PRESET_DEFAULT,
                           LZMA_CHECK_CRC32);
    */
    rv = lzma_alone_encoder(&lstream, &options);
  } else {
    /*
    rv = lzma_stream_decoder(
        &lstream,
        lzma_easy_decoder_memusage(LZMA_PRESET_DEFAULT),
        0);
    */
    rv = lzma_alone_decoder(&lstream, lzma_lzma_decoder_memusage(&options));
  }
  if (rv != LZMA_OK) {
    fprintf(stderr, "lzma_easy_encoder failed: %d\n", rv);
    return 0;
  }

  int success = 0;
  while (rv == LZMA_OK && success == 0) {
    if (ferror(in)) {
      fprintf(stderr, "Encountered error on input stream.\n");
      break;
    }
    if (ferror(out)) {
      fprintf(stderr, "Encountered error on output stream.\n");
      break;
    }

    if (lstream.avail_in <= 0 && feof(in) == 0) {
      // Need to read additional data from the input file.
      lstream.next_in = inbuf;
      lstream.avail_in = fread(inbuf, sizeof(inbuf[0]), kBufSize, in);
    }

    lzma_action action = LZMA_RUN;
    if (feof(in) != 0) {
      // We've reached EOF, so tell the encoder that we have no
      // additional data.
      action = LZMA_FINISH;
    }

    lstream.next_out = outbuf;
    lstream.avail_out = kBufSize;

    rv = lzma_code(&lstream, action);
    if (rv == LZMA_STREAM_END) {
      success = 1;
    }
    const size_t bytes_written = kBufSize - lstream.avail_out;
    if (bytes_written > 0) {
      fwrite(outbuf, sizeof(outbuf[0]), bytes_written, out);
    }
  }

  lzma_end(&lstream);

  if (success != 1) {
    fprintf(stderr, "lzma_code failed: %d\n", rv);
    return 0;
  }

  return 1;
}

int main(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <d|c> <infile> <outfile>\n", argv[0]);
    return 1;
  }
  const char* mode = argv[1];
  const char* infile = argv[2];
  const char* outfile = argv[3];

  if (strlen(mode) != 1) {
    fprintf(stderr, "Invalid mode %s\n", mode);
    return 1;
  }

  int compress;
  switch (mode[0]) {
    case 'd':
      compress = 0;
      break;
    case 'c':
      compress = 1;
      break;
    default:
      fprintf(stderr, "Invalid mode %s\n", mode);
      return 1;
  }

  FILE* in = fopen(infile, "rb");
  if (in == NULL) {
    fprintf(stderr, "Failed to open infile: %s\n", infile);
    return 1;
  }

  // See if the output file already exists. If so, abort.
  FILE* out = fopen(outfile, "rb");
  if (out != NULL) {
    fprintf(stderr, "Outfile exists: %s\n", outfile);
    fclose(out);
    fclose(in);
    return 1;
  }

  // Now open the output file for writing.
  out = fopen(outfile, "wb");
  if (out == NULL) {
    fprintf(stderr, "Failed to open outfile: %s\n", outfile);
    fclose(in);
    return 1;
  }

  fprintf(stderr, "%s %s to %s.\n",
          (compress ? "Compressing" : "Decompressing"),
          infile,
          outfile);
  int result = Compress(in, out, compress);

  fclose(in);
  fclose(out);

  if (result != 0) {
    fprintf(stderr, "Success!\n");
    return 0;
  } else {
    fprintf(stderr, "Failed.\n");
    remove(outfile);
    return 1;
  }
}
