#ifndef SNEPPX_DATA_PIPELINE_H
#define SNEPPX_DATA_PIPELINE_H

#include "subword_tokenization_pipeline.h"
#include "multidimensional_tensor_engine.h"
#include <stddef.h>

typedef struct SNEPPXDataPipeline SNEPPXDataPipeline;

SNEPPXDataPipeline* SNEPPX_data_pipeline_create(size_t batch_size);
void SNEPPX_data_pipeline_destroy(SNEPPXDataPipeline* pipe);
int SNEPPX_data_pipeline_load(const char* path, SNEPPXDataPipeline* pipe, SNEPPXTensor** data, SNEPPXTensor** labels);
int SNEPPX_data_pipeline_get_batch(SNEPPXDataPipeline* pipe, SNEPPXTensor** batch, SNEPPXTensor** labels);
void SNEPPX_data_pipeline_shuffle(SNEPPXDataPipeline* pipe);
size_t SNEPPX_data_pipeline_get_batch_size(const SNEPPXDataPipeline* pipe);

typedef struct SNEPPXTextDataset SNEPPXTextDataset;

SNEPPXTextDataset* SNEPPX_text_dataset_create(const char* path, SNEPPXTokenizer* tok,
                                            size_t seq_len, int line_by_line);
void SNEPPX_text_dataset_destroy(SNEPPXTextDataset* ds);

size_t SNEPPX_text_dataset_size(const SNEPPXTextDataset* ds);

int SNEPPX_text_dataset_get_batch(const SNEPPXTextDataset* ds, size_t start_idx, size_t batch_size,
                                 SNEPPXTensor** input_ids, SNEPPXTensor** target_ids);

#endif
