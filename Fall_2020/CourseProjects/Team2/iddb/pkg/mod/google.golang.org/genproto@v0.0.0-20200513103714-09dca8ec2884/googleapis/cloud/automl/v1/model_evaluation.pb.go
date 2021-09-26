// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/cloud/automl/v1/model_evaluation.proto

package automl

import (
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
	timestamp "github.com/golang/protobuf/ptypes/timestamp"
	_ "google.golang.org/genproto/googleapis/api/annotations"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

// Evaluation results of a model.
type ModelEvaluation struct {
	// Output only. Problem type specific evaluation metrics.
	//
	// Types that are valid to be assigned to Metrics:
	//	*ModelEvaluation_ClassificationEvaluationMetrics
	//	*ModelEvaluation_TranslationEvaluationMetrics
	//	*ModelEvaluation_ImageObjectDetectionEvaluationMetrics
	//	*ModelEvaluation_TextSentimentEvaluationMetrics
	//	*ModelEvaluation_TextExtractionEvaluationMetrics
	Metrics isModelEvaluation_Metrics `protobuf_oneof:"metrics"`
	// Output only. Resource name of the model evaluation.
	// Format:
	//
	// `projects/{project_id}/locations/{location_id}/models/{model_id}/modelEvaluations/{model_evaluation_id}`
	Name string `protobuf:"bytes,1,opt,name=name,proto3" json:"name,omitempty"`
	// Output only. The ID of the annotation spec that the model evaluation applies to. The
	// The ID is empty for the overall model evaluation.
	// For Tables annotation specs in the dataset do not exist and this ID is
	// always not set, but for CLASSIFICATION
	//
	// [prediction_type-s][google.cloud.automl.v1.TablesModelMetadata.prediction_type]
	// the
	// [display_name][google.cloud.automl.v1.ModelEvaluation.display_name]
	// field is used.
	AnnotationSpecId string `protobuf:"bytes,2,opt,name=annotation_spec_id,json=annotationSpecId,proto3" json:"annotation_spec_id,omitempty"`
	// Output only. The value of
	// [display_name][google.cloud.automl.v1.AnnotationSpec.display_name]
	// at the moment when the model was trained. Because this field returns a
	// value at model training time, for different models trained from the same
	// dataset, the values may differ, since display names could had been changed
	// between the two model's trainings. For Tables CLASSIFICATION
	//
	// [prediction_type-s][google.cloud.automl.v1.TablesModelMetadata.prediction_type]
	// distinct values of the target column at the moment of the model evaluation
	// are populated here.
	// The display_name is empty for the overall model evaluation.
	DisplayName string `protobuf:"bytes,15,opt,name=display_name,json=displayName,proto3" json:"display_name,omitempty"`
	// Output only. Timestamp when this model evaluation was created.
	CreateTime *timestamp.Timestamp `protobuf:"bytes,5,opt,name=create_time,json=createTime,proto3" json:"create_time,omitempty"`
	// Output only. The number of examples used for model evaluation, i.e. for
	// which ground truth from time of model creation is compared against the
	// predicted annotations created by the model.
	// For overall ModelEvaluation (i.e. with annotation_spec_id not set) this is
	// the total number of all examples used for evaluation.
	// Otherwise, this is the count of examples that according to the ground
	// truth were annotated by the
	//
	// [annotation_spec_id][google.cloud.automl.v1.ModelEvaluation.annotation_spec_id].
	EvaluatedExampleCount int32    `protobuf:"varint,6,opt,name=evaluated_example_count,json=evaluatedExampleCount,proto3" json:"evaluated_example_count,omitempty"`
	XXX_NoUnkeyedLiteral  struct{} `json:"-"`
	XXX_unrecognized      []byte   `json:"-"`
	XXX_sizecache         int32    `json:"-"`
}

func (m *ModelEvaluation) Reset()         { *m = ModelEvaluation{} }
func (m *ModelEvaluation) String() string { return proto.CompactTextString(m) }
func (*ModelEvaluation) ProtoMessage()    {}
func (*ModelEvaluation) Descriptor() ([]byte, []int) {
	return fileDescriptor_008481175b84a2ca, []int{0}
}

func (m *ModelEvaluation) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_ModelEvaluation.Unmarshal(m, b)
}
func (m *ModelEvaluation) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_ModelEvaluation.Marshal(b, m, deterministic)
}
func (m *ModelEvaluation) XXX_Merge(src proto.Message) {
	xxx_messageInfo_ModelEvaluation.Merge(m, src)
}
func (m *ModelEvaluation) XXX_Size() int {
	return xxx_messageInfo_ModelEvaluation.Size(m)
}
func (m *ModelEvaluation) XXX_DiscardUnknown() {
	xxx_messageInfo_ModelEvaluation.DiscardUnknown(m)
}

var xxx_messageInfo_ModelEvaluation proto.InternalMessageInfo

type isModelEvaluation_Metrics interface {
	isModelEvaluation_Metrics()
}

type ModelEvaluation_ClassificationEvaluationMetrics struct {
	ClassificationEvaluationMetrics *ClassificationEvaluationMetrics `protobuf:"bytes,8,opt,name=classification_evaluation_metrics,json=classificationEvaluationMetrics,proto3,oneof"`
}

type ModelEvaluation_TranslationEvaluationMetrics struct {
	TranslationEvaluationMetrics *TranslationEvaluationMetrics `protobuf:"bytes,9,opt,name=translation_evaluation_metrics,json=translationEvaluationMetrics,proto3,oneof"`
}

type ModelEvaluation_ImageObjectDetectionEvaluationMetrics struct {
	ImageObjectDetectionEvaluationMetrics *ImageObjectDetectionEvaluationMetrics `protobuf:"bytes,12,opt,name=image_object_detection_evaluation_metrics,json=imageObjectDetectionEvaluationMetrics,proto3,oneof"`
}

type ModelEvaluation_TextSentimentEvaluationMetrics struct {
	TextSentimentEvaluationMetrics *TextSentimentEvaluationMetrics `protobuf:"bytes,11,opt,name=text_sentiment_evaluation_metrics,json=textSentimentEvaluationMetrics,proto3,oneof"`
}

type ModelEvaluation_TextExtractionEvaluationMetrics struct {
	TextExtractionEvaluationMetrics *TextExtractionEvaluationMetrics `protobuf:"bytes,13,opt,name=text_extraction_evaluation_metrics,json=textExtractionEvaluationMetrics,proto3,oneof"`
}

func (*ModelEvaluation_ClassificationEvaluationMetrics) isModelEvaluation_Metrics() {}

func (*ModelEvaluation_TranslationEvaluationMetrics) isModelEvaluation_Metrics() {}

func (*ModelEvaluation_ImageObjectDetectionEvaluationMetrics) isModelEvaluation_Metrics() {}

func (*ModelEvaluation_TextSentimentEvaluationMetrics) isModelEvaluation_Metrics() {}

func (*ModelEvaluation_TextExtractionEvaluationMetrics) isModelEvaluation_Metrics() {}

func (m *ModelEvaluation) GetMetrics() isModelEvaluation_Metrics {
	if m != nil {
		return m.Metrics
	}
	return nil
}

func (m *ModelEvaluation) GetClassificationEvaluationMetrics() *ClassificationEvaluationMetrics {
	if x, ok := m.GetMetrics().(*ModelEvaluation_ClassificationEvaluationMetrics); ok {
		return x.ClassificationEvaluationMetrics
	}
	return nil
}

func (m *ModelEvaluation) GetTranslationEvaluationMetrics() *TranslationEvaluationMetrics {
	if x, ok := m.GetMetrics().(*ModelEvaluation_TranslationEvaluationMetrics); ok {
		return x.TranslationEvaluationMetrics
	}
	return nil
}

func (m *ModelEvaluation) GetImageObjectDetectionEvaluationMetrics() *ImageObjectDetectionEvaluationMetrics {
	if x, ok := m.GetMetrics().(*ModelEvaluation_ImageObjectDetectionEvaluationMetrics); ok {
		return x.ImageObjectDetectionEvaluationMetrics
	}
	return nil
}

func (m *ModelEvaluation) GetTextSentimentEvaluationMetrics() *TextSentimentEvaluationMetrics {
	if x, ok := m.GetMetrics().(*ModelEvaluation_TextSentimentEvaluationMetrics); ok {
		return x.TextSentimentEvaluationMetrics
	}
	return nil
}

func (m *ModelEvaluation) GetTextExtractionEvaluationMetrics() *TextExtractionEvaluationMetrics {
	if x, ok := m.GetMetrics().(*ModelEvaluation_TextExtractionEvaluationMetrics); ok {
		return x.TextExtractionEvaluationMetrics
	}
	return nil
}

func (m *ModelEvaluation) GetName() string {
	if m != nil {
		return m.Name
	}
	return ""
}

func (m *ModelEvaluation) GetAnnotationSpecId() string {
	if m != nil {
		return m.AnnotationSpecId
	}
	return ""
}

func (m *ModelEvaluation) GetDisplayName() string {
	if m != nil {
		return m.DisplayName
	}
	return ""
}

func (m *ModelEvaluation) GetCreateTime() *timestamp.Timestamp {
	if m != nil {
		return m.CreateTime
	}
	return nil
}

func (m *ModelEvaluation) GetEvaluatedExampleCount() int32 {
	if m != nil {
		return m.EvaluatedExampleCount
	}
	return 0
}

// XXX_OneofWrappers is for the internal use of the proto package.
func (*ModelEvaluation) XXX_OneofWrappers() []interface{} {
	return []interface{}{
		(*ModelEvaluation_ClassificationEvaluationMetrics)(nil),
		(*ModelEvaluation_TranslationEvaluationMetrics)(nil),
		(*ModelEvaluation_ImageObjectDetectionEvaluationMetrics)(nil),
		(*ModelEvaluation_TextSentimentEvaluationMetrics)(nil),
		(*ModelEvaluation_TextExtractionEvaluationMetrics)(nil),
	}
}

func init() {
	proto.RegisterType((*ModelEvaluation)(nil), "google.cloud.automl.v1.ModelEvaluation")
}

func init() {
	proto.RegisterFile("google/cloud/automl/v1/model_evaluation.proto", fileDescriptor_008481175b84a2ca)
}

var fileDescriptor_008481175b84a2ca = []byte{
	// 643 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x8c, 0x54, 0xdd, 0x4e, 0xd4, 0x40,
	0x18, 0xb5, 0x44, 0x10, 0x66, 0x31, 0x98, 0x49, 0xc4, 0x65, 0x43, 0xf8, 0x4b, 0x30, 0x6b, 0xc0,
	0x4e, 0x56, 0x0d, 0x26, 0x45, 0x2f, 0x00, 0x89, 0x92, 0x88, 0x9a, 0x85, 0x70, 0x41, 0x48, 0x9a,
	0x61, 0xfa, 0xd1, 0xd4, 0x4c, 0x3b, 0x4d, 0x67, 0x4a, 0xd6, 0x20, 0x57, 0x44, 0x7d, 0x06, 0x5f,
	0x81, 0x3b, 0x5f, 0xc3, 0x47, 0xe1, 0x29, 0x4c, 0x67, 0xda, 0xdd, 0x42, 0xda, 0xc6, 0xab, 0x4e,
	0x7b, 0xce, 0x77, 0xe6, 0xcc, 0xf9, 0xbe, 0x29, 0x7a, 0xee, 0x0b, 0xe1, 0x73, 0x20, 0x8c, 0x8b,
	0xd4, 0x23, 0x34, 0x55, 0x22, 0xe4, 0xe4, 0xbc, 0x47, 0x42, 0xe1, 0x01, 0x77, 0xe1, 0x9c, 0xf2,
	0x94, 0xaa, 0x40, 0x44, 0x76, 0x9c, 0x08, 0x25, 0xf0, 0xac, 0xa1, 0xdb, 0x9a, 0x6e, 0x1b, 0xba,
	0x7d, 0xde, 0xeb, 0xcc, 0xe5, 0x32, 0x34, 0x0e, 0x48, 0x02, 0x52, 0xa4, 0x09, 0x03, 0x53, 0xd2,
	0x59, 0xab, 0xd9, 0x81, 0x71, 0x2a, 0x65, 0x70, 0x16, 0xb0, 0x92, 0x7e, 0xe7, 0x69, 0x0d, 0xd9,
	0x03, 0x05, 0xac, 0xc4, 0x5b, 0xaf, 0xe1, 0x29, 0x18, 0x28, 0x17, 0x06, 0x2a, 0xa1, 0x65, 0xf6,
	0x5a, 0x13, 0x5b, 0x42, 0xa4, 0x82, 0x10, 0x22, 0x95, 0x93, 0xbb, 0x75, 0xe4, 0x84, 0x46, 0x92,
	0x97, 0xcd, 0x2e, 0xe6, 0x4c, 0xfd, 0x76, 0x9a, 0x9e, 0x91, 0x4c, 0x47, 0x2a, 0x1a, 0xc6, 0x39,
	0x61, 0xbe, 0x94, 0x0a, 0x8d, 0x22, 0xa1, 0x74, 0xb5, 0x34, 0xe8, 0xca, 0x9f, 0x49, 0x34, 0xb3,
	0x9f, 0xc5, 0xbc, 0x3b, 0x4c, 0x19, 0xff, 0xb0, 0xd0, 0xf2, 0xed, 0x60, 0x4a, 0x3d, 0x70, 0x43,
	0x50, 0x49, 0xc0, 0x64, 0x7b, 0x72, 0xc9, 0xea, 0xb6, 0x5e, 0xbc, 0xb6, 0xab, 0x9b, 0x61, 0xef,
	0xdc, 0x12, 0x18, 0xa9, 0xef, 0x9b, 0xf2, 0x0f, 0xf7, 0xfa, 0x8b, 0xac, 0x99, 0x82, 0xbf, 0xa3,
	0x85, 0xd2, 0x79, 0xab, 0x3c, 0x4c, 0x69, 0x0f, 0xaf, 0xea, 0x3c, 0x1c, 0x8e, 0xaa, 0xab, 0x0c,
	0xcc, 0xab, 0x06, 0x1c, 0xff, 0xb6, 0xd0, 0xb3, 0x20, 0xa4, 0x3e, 0xb8, 0xe2, 0xf4, 0x2b, 0x30,
	0xe5, 0x0e, 0xdb, 0x5f, 0xe5, 0x64, 0x5a, 0x3b, 0x79, 0x5b, 0xe7, 0x64, 0x2f, 0x13, 0xfa, 0xac,
	0x75, 0xde, 0x15, 0x32, 0x55, 0x96, 0x56, 0x83, 0xff, 0x21, 0xe2, 0x2b, 0x0b, 0x2d, 0xdf, 0x9e,
	0x9b, 0x2a, 0x4f, 0x2d, 0xed, 0x69, 0xa3, 0x36, 0x1d, 0x18, 0xa8, 0x83, 0xa2, 0xbe, 0xca, 0xcc,
	0x82, 0x6a, 0x64, 0xe0, 0x9f, 0x16, 0x5a, 0xb9, 0x33, 0xeb, 0x55, 0x36, 0x1e, 0x36, 0x0f, 0x4a,
	0x66, 0x63, 0x77, 0x28, 0x50, 0x39, 0x28, 0xaa, 0x99, 0x82, 0x31, 0xba, 0x1f, 0xd1, 0x10, 0xda,
	0xd6, 0x92, 0xd5, 0x9d, 0xea, 0xeb, 0x35, 0x5e, 0x47, 0x78, 0x34, 0xed, 0xae, 0x8c, 0x81, 0xb9,
	0x81, 0xd7, 0x1e, 0xd3, 0x8c, 0x47, 0x23, 0xe4, 0x20, 0x06, 0xb6, 0xe7, 0xe1, 0x65, 0x34, 0xed,
	0x05, 0x32, 0xe6, 0xf4, 0x9b, 0xab, 0x95, 0x66, 0x34, 0xaf, 0x95, 0x7f, 0xfb, 0x94, 0x09, 0x6e,
	0xa2, 0x16, 0x4b, 0x80, 0x2a, 0x70, 0xb3, 0x3c, 0xda, 0xe3, 0xfa, 0x54, 0x9d, 0xe2, 0x54, 0xc5,
	0xf5, 0xb3, 0x0f, 0x8b, 0xeb, 0xd7, 0x47, 0x86, 0x9e, 0x7d, 0xc0, 0x1b, 0xe8, 0x49, 0x9e, 0x0c,
	0x78, 0x2e, 0x0c, 0x68, 0x18, 0x73, 0x70, 0x99, 0x48, 0x23, 0xd5, 0x9e, 0x58, 0xb2, 0xba, 0xe3,
	0xfd, 0xc7, 0x43, 0x78, 0xd7, 0xa0, 0x3b, 0x19, 0xe8, 0xfc, 0xb2, 0x6e, 0xb6, 0xae, 0x2c, 0xb4,
	0x9a, 0xc7, 0x65, 0x36, 0xa3, 0x71, 0x20, 0x6d, 0x26, 0x42, 0x72, 0xf7, 0xe2, 0x1e, 0xc7, 0x89,
	0xc8, 0x26, 0x47, 0x92, 0x8b, 0x7c, 0x75, 0x49, 0xb8, 0x30, 0xd7, 0x4b, 0x92, 0x8b, 0x62, 0x79,
	0x69, 0x7e, 0xac, 0x92, 0x5c, 0xe8, 0x67, 0xfe, 0x3a, 0xd2, 0x29, 0x80, 0x52, 0x33, 0x2f, 0xb7,
	0xa7, 0xd0, 0x83, 0xbc, 0xa1, 0xdb, 0xd7, 0x16, 0xea, 0x30, 0x11, 0xd6, 0x34, 0xf4, 0x8b, 0x75,
	0xfc, 0x26, 0x47, 0x7c, 0xc1, 0x69, 0xe4, 0xdb, 0x22, 0xf1, 0x89, 0x0f, 0x91, 0x8e, 0x88, 0x8c,
	0x8e, 0x70, 0xf7, 0xe7, 0xb6, 0x69, 0x56, 0xd7, 0x63, 0xb3, 0xef, 0x4d, 0xf9, 0x8e, 0x16, 0xde,
	0x4a, 0x95, 0xd8, 0xff, 0x68, 0x1f, 0xf5, 0xfe, 0x16, 0xc0, 0x89, 0x06, 0x4e, 0x34, 0xc0, 0x4f,
	0x8e, 0x7a, 0x37, 0x63, 0x73, 0x06, 0x70, 0x1c, 0x8d, 0x38, 0x8e, 0xa9, 0x71, 0x9c, 0xa3, 0xde,
	0xe9, 0x84, 0xde, 0xf6, 0xe5, 0xbf, 0x00, 0x00, 0x00, 0xff, 0xff, 0x18, 0xf0, 0xbd, 0x56, 0x64,
	0x06, 0x00, 0x00,
}
