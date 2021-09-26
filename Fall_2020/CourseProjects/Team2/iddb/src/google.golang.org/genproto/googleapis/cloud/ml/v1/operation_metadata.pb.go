// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.22.0
// 	protoc        v3.12.3
// source: google/cloud/ml/v1/operation_metadata.proto

package ml

import (
	reflect "reflect"
	sync "sync"

	proto "github.com/golang/protobuf/proto"
	timestamp "github.com/golang/protobuf/ptypes/timestamp"
	_ "google.golang.org/genproto/googleapis/api/annotations"
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
)

const (
	// Verify that this generated code is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(20 - protoimpl.MinVersion)
	// Verify that runtime/protoimpl is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(protoimpl.MaxVersion - 20)
)

// This is a compile-time assertion that a sufficiently up-to-date version
// of the legacy proto package is being used.
const _ = proto.ProtoPackageIsVersion4

// The operation type.
type OperationMetadata_OperationType int32

const (
	// Unspecified operation type.
	OperationMetadata_OPERATION_TYPE_UNSPECIFIED OperationMetadata_OperationType = 0
	// An operation to create a new version.
	OperationMetadata_CREATE_VERSION OperationMetadata_OperationType = 1
	// An operation to delete an existing version.
	OperationMetadata_DELETE_VERSION OperationMetadata_OperationType = 2
	// An operation to delete an existing model.
	OperationMetadata_DELETE_MODEL OperationMetadata_OperationType = 3
)

// Enum value maps for OperationMetadata_OperationType.
var (
	OperationMetadata_OperationType_name = map[int32]string{
		0: "OPERATION_TYPE_UNSPECIFIED",
		1: "CREATE_VERSION",
		2: "DELETE_VERSION",
		3: "DELETE_MODEL",
	}
	OperationMetadata_OperationType_value = map[string]int32{
		"OPERATION_TYPE_UNSPECIFIED": 0,
		"CREATE_VERSION":             1,
		"DELETE_VERSION":             2,
		"DELETE_MODEL":               3,
	}
)

func (x OperationMetadata_OperationType) Enum() *OperationMetadata_OperationType {
	p := new(OperationMetadata_OperationType)
	*p = x
	return p
}

func (x OperationMetadata_OperationType) String() string {
	return protoimpl.X.EnumStringOf(x.Descriptor(), protoreflect.EnumNumber(x))
}

func (OperationMetadata_OperationType) Descriptor() protoreflect.EnumDescriptor {
	return file_google_cloud_ml_v1_operation_metadata_proto_enumTypes[0].Descriptor()
}

func (OperationMetadata_OperationType) Type() protoreflect.EnumType {
	return &file_google_cloud_ml_v1_operation_metadata_proto_enumTypes[0]
}

func (x OperationMetadata_OperationType) Number() protoreflect.EnumNumber {
	return protoreflect.EnumNumber(x)
}

// Deprecated: Use OperationMetadata_OperationType.Descriptor instead.
func (OperationMetadata_OperationType) EnumDescriptor() ([]byte, []int) {
	return file_google_cloud_ml_v1_operation_metadata_proto_rawDescGZIP(), []int{0, 0}
}

// Represents the metadata of the long-running operation.
type OperationMetadata struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	// The time the operation was submitted.
	CreateTime *timestamp.Timestamp `protobuf:"bytes,1,opt,name=create_time,json=createTime,proto3" json:"create_time,omitempty"`
	// The time operation processing started.
	StartTime *timestamp.Timestamp `protobuf:"bytes,2,opt,name=start_time,json=startTime,proto3" json:"start_time,omitempty"`
	// The time operation processing completed.
	EndTime *timestamp.Timestamp `protobuf:"bytes,3,opt,name=end_time,json=endTime,proto3" json:"end_time,omitempty"`
	// Indicates whether a request to cancel this operation has been made.
	IsCancellationRequested bool `protobuf:"varint,4,opt,name=is_cancellation_requested,json=isCancellationRequested,proto3" json:"is_cancellation_requested,omitempty"`
	// The operation type.
	OperationType OperationMetadata_OperationType `protobuf:"varint,5,opt,name=operation_type,json=operationType,proto3,enum=google.cloud.ml.v1.OperationMetadata_OperationType" json:"operation_type,omitempty"`
	// Contains the name of the model associated with the operation.
	ModelName string `protobuf:"bytes,6,opt,name=model_name,json=modelName,proto3" json:"model_name,omitempty"`
	// Contains the version associated with the operation.
	Version *Version `protobuf:"bytes,7,opt,name=version,proto3" json:"version,omitempty"`
}

func (x *OperationMetadata) Reset() {
	*x = OperationMetadata{}
	if protoimpl.UnsafeEnabled {
		mi := &file_google_cloud_ml_v1_operation_metadata_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *OperationMetadata) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*OperationMetadata) ProtoMessage() {}

func (x *OperationMetadata) ProtoReflect() protoreflect.Message {
	mi := &file_google_cloud_ml_v1_operation_metadata_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use OperationMetadata.ProtoReflect.Descriptor instead.
func (*OperationMetadata) Descriptor() ([]byte, []int) {
	return file_google_cloud_ml_v1_operation_metadata_proto_rawDescGZIP(), []int{0}
}

func (x *OperationMetadata) GetCreateTime() *timestamp.Timestamp {
	if x != nil {
		return x.CreateTime
	}
	return nil
}

func (x *OperationMetadata) GetStartTime() *timestamp.Timestamp {
	if x != nil {
		return x.StartTime
	}
	return nil
}

func (x *OperationMetadata) GetEndTime() *timestamp.Timestamp {
	if x != nil {
		return x.EndTime
	}
	return nil
}

func (x *OperationMetadata) GetIsCancellationRequested() bool {
	if x != nil {
		return x.IsCancellationRequested
	}
	return false
}

func (x *OperationMetadata) GetOperationType() OperationMetadata_OperationType {
	if x != nil {
		return x.OperationType
	}
	return OperationMetadata_OPERATION_TYPE_UNSPECIFIED
}

func (x *OperationMetadata) GetModelName() string {
	if x != nil {
		return x.ModelName
	}
	return ""
}

func (x *OperationMetadata) GetVersion() *Version {
	if x != nil {
		return x.Version
	}
	return nil
}

var File_google_cloud_ml_v1_operation_metadata_proto protoreflect.FileDescriptor

var file_google_cloud_ml_v1_operation_metadata_proto_rawDesc = []byte{
	0x0a, 0x2b, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2f, 0x63, 0x6c, 0x6f, 0x75, 0x64, 0x2f, 0x6d,
	0x6c, 0x2f, 0x76, 0x31, 0x2f, 0x6f, 0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x5f, 0x6d,
	0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x12, 0x12, 0x67,
	0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x63, 0x6c, 0x6f, 0x75, 0x64, 0x2e, 0x6d, 0x6c, 0x2e, 0x76,
	0x31, 0x1a, 0x1c, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2f, 0x61, 0x70, 0x69, 0x2f, 0x61, 0x6e,
	0x6e, 0x6f, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x1a,
	0x26, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2f, 0x63, 0x6c, 0x6f, 0x75, 0x64, 0x2f, 0x6d, 0x6c,
	0x2f, 0x76, 0x31, 0x2f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x5f, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63,
	0x65, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x1a, 0x1f, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2f,
	0x70, 0x72, 0x6f, 0x74, 0x6f, 0x62, 0x75, 0x66, 0x2f, 0x74, 0x69, 0x6d, 0x65, 0x73, 0x74, 0x61,
	0x6d, 0x70, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x22, 0x9b, 0x04, 0x0a, 0x11, 0x4f, 0x70, 0x65,
	0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x4d, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x12, 0x3b,
	0x0a, 0x0b, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x5f, 0x74, 0x69, 0x6d, 0x65, 0x18, 0x01, 0x20,
	0x01, 0x28, 0x0b, 0x32, 0x1a, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x70, 0x72, 0x6f,
	0x74, 0x6f, 0x62, 0x75, 0x66, 0x2e, 0x54, 0x69, 0x6d, 0x65, 0x73, 0x74, 0x61, 0x6d, 0x70, 0x52,
	0x0a, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x54, 0x69, 0x6d, 0x65, 0x12, 0x39, 0x0a, 0x0a, 0x73,
	0x74, 0x61, 0x72, 0x74, 0x5f, 0x74, 0x69, 0x6d, 0x65, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0b, 0x32,
	0x1a, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x62, 0x75,
	0x66, 0x2e, 0x54, 0x69, 0x6d, 0x65, 0x73, 0x74, 0x61, 0x6d, 0x70, 0x52, 0x09, 0x73, 0x74, 0x61,
	0x72, 0x74, 0x54, 0x69, 0x6d, 0x65, 0x12, 0x35, 0x0a, 0x08, 0x65, 0x6e, 0x64, 0x5f, 0x74, 0x69,
	0x6d, 0x65, 0x18, 0x03, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1a, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c,
	0x65, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x62, 0x75, 0x66, 0x2e, 0x54, 0x69, 0x6d, 0x65, 0x73,
	0x74, 0x61, 0x6d, 0x70, 0x52, 0x07, 0x65, 0x6e, 0x64, 0x54, 0x69, 0x6d, 0x65, 0x12, 0x3a, 0x0a,
	0x19, 0x69, 0x73, 0x5f, 0x63, 0x61, 0x6e, 0x63, 0x65, 0x6c, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e,
	0x5f, 0x72, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x65, 0x64, 0x18, 0x04, 0x20, 0x01, 0x28, 0x08,
	0x52, 0x17, 0x69, 0x73, 0x43, 0x61, 0x6e, 0x63, 0x65, 0x6c, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e,
	0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x65, 0x64, 0x12, 0x5a, 0x0a, 0x0e, 0x6f, 0x70, 0x65,
	0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x18, 0x05, 0x20, 0x01, 0x28,
	0x0e, 0x32, 0x33, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x63, 0x6c, 0x6f, 0x75, 0x64,
	0x2e, 0x6d, 0x6c, 0x2e, 0x76, 0x31, 0x2e, 0x4f, 0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e,
	0x4d, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x2e, 0x4f, 0x70, 0x65, 0x72, 0x61, 0x74, 0x69,
	0x6f, 0x6e, 0x54, 0x79, 0x70, 0x65, 0x52, 0x0d, 0x6f, 0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6f,
	0x6e, 0x54, 0x79, 0x70, 0x65, 0x12, 0x1d, 0x0a, 0x0a, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x5f, 0x6e,
	0x61, 0x6d, 0x65, 0x18, 0x06, 0x20, 0x01, 0x28, 0x09, 0x52, 0x09, 0x6d, 0x6f, 0x64, 0x65, 0x6c,
	0x4e, 0x61, 0x6d, 0x65, 0x12, 0x35, 0x0a, 0x07, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x18,
	0x07, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1b, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x63,
	0x6c, 0x6f, 0x75, 0x64, 0x2e, 0x6d, 0x6c, 0x2e, 0x76, 0x31, 0x2e, 0x56, 0x65, 0x72, 0x73, 0x69,
	0x6f, 0x6e, 0x52, 0x07, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x22, 0x69, 0x0a, 0x0d, 0x4f,
	0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x54, 0x79, 0x70, 0x65, 0x12, 0x1e, 0x0a, 0x1a,
	0x4f, 0x50, 0x45, 0x52, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x5f, 0x54, 0x59, 0x50, 0x45, 0x5f, 0x55,
	0x4e, 0x53, 0x50, 0x45, 0x43, 0x49, 0x46, 0x49, 0x45, 0x44, 0x10, 0x00, 0x12, 0x12, 0x0a, 0x0e,
	0x43, 0x52, 0x45, 0x41, 0x54, 0x45, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x10, 0x01,
	0x12, 0x12, 0x0a, 0x0e, 0x44, 0x45, 0x4c, 0x45, 0x54, 0x45, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49,
	0x4f, 0x4e, 0x10, 0x02, 0x12, 0x10, 0x0a, 0x0c, 0x44, 0x45, 0x4c, 0x45, 0x54, 0x45, 0x5f, 0x4d,
	0x4f, 0x44, 0x45, 0x4c, 0x10, 0x03, 0x42, 0x6c, 0x0a, 0x1a, 0x63, 0x6f, 0x6d, 0x2e, 0x67, 0x6f,
	0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x63, 0x6c, 0x6f, 0x75, 0x64, 0x2e, 0x6d, 0x6c, 0x2e, 0x61, 0x70,
	0x69, 0x2e, 0x76, 0x31, 0x42, 0x16, 0x4f, 0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x4d,
	0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x50, 0x72, 0x6f, 0x74, 0x6f, 0x50, 0x01, 0x5a, 0x34,
	0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x67, 0x6f, 0x6c, 0x61, 0x6e, 0x67, 0x2e, 0x6f, 0x72,
	0x67, 0x2f, 0x67, 0x65, 0x6e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x2f, 0x67, 0x6f, 0x6f, 0x67, 0x6c,
	0x65, 0x61, 0x70, 0x69, 0x73, 0x2f, 0x63, 0x6c, 0x6f, 0x75, 0x64, 0x2f, 0x6d, 0x6c, 0x2f, 0x76,
	0x31, 0x3b, 0x6d, 0x6c, 0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_google_cloud_ml_v1_operation_metadata_proto_rawDescOnce sync.Once
	file_google_cloud_ml_v1_operation_metadata_proto_rawDescData = file_google_cloud_ml_v1_operation_metadata_proto_rawDesc
)

func file_google_cloud_ml_v1_operation_metadata_proto_rawDescGZIP() []byte {
	file_google_cloud_ml_v1_operation_metadata_proto_rawDescOnce.Do(func() {
		file_google_cloud_ml_v1_operation_metadata_proto_rawDescData = protoimpl.X.CompressGZIP(file_google_cloud_ml_v1_operation_metadata_proto_rawDescData)
	})
	return file_google_cloud_ml_v1_operation_metadata_proto_rawDescData
}

var file_google_cloud_ml_v1_operation_metadata_proto_enumTypes = make([]protoimpl.EnumInfo, 1)
var file_google_cloud_ml_v1_operation_metadata_proto_msgTypes = make([]protoimpl.MessageInfo, 1)
var file_google_cloud_ml_v1_operation_metadata_proto_goTypes = []interface{}{
	(OperationMetadata_OperationType)(0), // 0: google.cloud.ml.v1.OperationMetadata.OperationType
	(*OperationMetadata)(nil),            // 1: google.cloud.ml.v1.OperationMetadata
	(*timestamp.Timestamp)(nil),          // 2: google.protobuf.Timestamp
	(*Version)(nil),                      // 3: google.cloud.ml.v1.Version
}
var file_google_cloud_ml_v1_operation_metadata_proto_depIdxs = []int32{
	2, // 0: google.cloud.ml.v1.OperationMetadata.create_time:type_name -> google.protobuf.Timestamp
	2, // 1: google.cloud.ml.v1.OperationMetadata.start_time:type_name -> google.protobuf.Timestamp
	2, // 2: google.cloud.ml.v1.OperationMetadata.end_time:type_name -> google.protobuf.Timestamp
	0, // 3: google.cloud.ml.v1.OperationMetadata.operation_type:type_name -> google.cloud.ml.v1.OperationMetadata.OperationType
	3, // 4: google.cloud.ml.v1.OperationMetadata.version:type_name -> google.cloud.ml.v1.Version
	5, // [5:5] is the sub-list for method output_type
	5, // [5:5] is the sub-list for method input_type
	5, // [5:5] is the sub-list for extension type_name
	5, // [5:5] is the sub-list for extension extendee
	0, // [0:5] is the sub-list for field type_name
}

func init() { file_google_cloud_ml_v1_operation_metadata_proto_init() }
func file_google_cloud_ml_v1_operation_metadata_proto_init() {
	if File_google_cloud_ml_v1_operation_metadata_proto != nil {
		return
	}
	file_google_cloud_ml_v1_model_service_proto_init()
	if !protoimpl.UnsafeEnabled {
		file_google_cloud_ml_v1_operation_metadata_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*OperationMetadata); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: file_google_cloud_ml_v1_operation_metadata_proto_rawDesc,
			NumEnums:      1,
			NumMessages:   1,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_google_cloud_ml_v1_operation_metadata_proto_goTypes,
		DependencyIndexes: file_google_cloud_ml_v1_operation_metadata_proto_depIdxs,
		EnumInfos:         file_google_cloud_ml_v1_operation_metadata_proto_enumTypes,
		MessageInfos:      file_google_cloud_ml_v1_operation_metadata_proto_msgTypes,
	}.Build()
	File_google_cloud_ml_v1_operation_metadata_proto = out.File
	file_google_cloud_ml_v1_operation_metadata_proto_rawDesc = nil
	file_google_cloud_ml_v1_operation_metadata_proto_goTypes = nil
	file_google_cloud_ml_v1_operation_metadata_proto_depIdxs = nil
}
