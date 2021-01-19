// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/cloud/datacatalog/v1/common.proto

package datacatalog

import (
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
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

// This enum describes all the possible systems that Data Catalog integrates
// with.
type IntegratedSystem int32

const (
	// Default unknown system.
	IntegratedSystem_INTEGRATED_SYSTEM_UNSPECIFIED IntegratedSystem = 0
	// BigQuery.
	IntegratedSystem_BIGQUERY IntegratedSystem = 1
	// Cloud Pub/Sub.
	IntegratedSystem_CLOUD_PUBSUB IntegratedSystem = 2
)

var IntegratedSystem_name = map[int32]string{
	0: "INTEGRATED_SYSTEM_UNSPECIFIED",
	1: "BIGQUERY",
	2: "CLOUD_PUBSUB",
}

var IntegratedSystem_value = map[string]int32{
	"INTEGRATED_SYSTEM_UNSPECIFIED": 0,
	"BIGQUERY":                      1,
	"CLOUD_PUBSUB":                  2,
}

func (x IntegratedSystem) String() string {
	return proto.EnumName(IntegratedSystem_name, int32(x))
}

func (IntegratedSystem) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_fec73966e023eb05, []int{0}
}

func init() {
	proto.RegisterEnum("google.cloud.datacatalog.v1.IntegratedSystem", IntegratedSystem_name, IntegratedSystem_value)
}

func init() {
	proto.RegisterFile("google/cloud/datacatalog/v1/common.proto", fileDescriptor_fec73966e023eb05)
}

var fileDescriptor_fec73966e023eb05 = []byte{
	// 266 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x7c, 0xcf, 0xc1, 0x4a, 0xc3, 0x30,
	0x1c, 0x06, 0x70, 0xbb, 0x83, 0x48, 0xd8, 0xa1, 0xf4, 0x3a, 0x54, 0xbc, 0x89, 0x87, 0x84, 0xe2,
	0x2d, 0x9e, 0x6c, 0x9b, 0x95, 0x80, 0xce, 0xba, 0x34, 0x85, 0x49, 0xa1, 0xc4, 0x36, 0x04, 0xa1,
	0xed, 0x7f, 0x74, 0x71, 0xe0, 0x2b, 0xf9, 0x28, 0xfa, 0x26, 0x3e, 0x81, 0x47, 0x21, 0x9d, 0xd0,
	0x83, 0x78, 0x0c, 0xbf, 0x8f, 0x7c, 0xdf, 0x1f, 0x5d, 0x1a, 0x00, 0xd3, 0x6a, 0x52, 0xb7, 0xf0,
	0xda, 0x90, 0x46, 0x59, 0x55, 0x2b, 0xab, 0x5a, 0x30, 0x64, 0x1f, 0x92, 0x1a, 0xba, 0x0e, 0x7a,
	0xbc, 0x1d, 0xc0, 0x42, 0xb0, 0x18, 0x93, 0xd8, 0x25, 0xf1, 0x24, 0x89, 0xf7, 0xe1, 0x95, 0x44,
	0x3e, 0xef, 0xad, 0x36, 0x83, 0xb2, 0xba, 0x11, 0x6f, 0x3b, 0xab, 0xbb, 0xe0, 0x02, 0x9d, 0xf2,
	0x55, 0xce, 0xd2, 0xf5, 0x6d, 0xce, 0x92, 0x4a, 0x6c, 0x44, 0xce, 0xee, 0x2b, 0xb9, 0x12, 0x19,
	0x8b, 0xf9, 0x92, 0xb3, 0xc4, 0x3f, 0x0a, 0xe6, 0xe8, 0x24, 0xe2, 0xe9, 0xa3, 0x64, 0xeb, 0x8d,
	0xef, 0x05, 0x3e, 0x9a, 0xc7, 0x77, 0x0f, 0x32, 0xa9, 0x32, 0x19, 0x09, 0x19, 0xf9, 0xb3, 0xe8,
	0xd3, 0x43, 0xe7, 0x35, 0x74, 0xf8, 0x9f, 0xea, 0xcc, 0x7b, 0x5a, 0x1e, 0xd8, 0x40, 0xab, 0x7a,
	0x83, 0x61, 0x30, 0xc4, 0xe8, 0xde, 0xad, 0x26, 0x23, 0xa9, 0xed, 0xcb, 0xee, 0xcf, 0x13, 0x6f,
	0x26, 0xcf, 0x6f, 0xcf, 0x7b, 0x9f, 0x2d, 0xd2, 0xf1, 0xaf, 0xd8, 0x55, 0x25, 0xca, 0xaa, 0xf8,
	0x50, 0x55, 0x84, 0x1f, 0xbf, 0x5a, 0x3a, 0x2d, 0x27, 0x5a, 0x16, 0xe1, 0xd7, 0xec, 0x6c, 0x54,
	0x4a, 0x1d, 0x53, 0x3a, 0x71, 0x4a, 0x8b, 0xf0, 0xf9, 0xd8, 0x4d, 0xba, 0xfe, 0x09, 0x00, 0x00,
	0xff, 0xff, 0xf1, 0x30, 0x6e, 0xe1, 0x74, 0x01, 0x00, 0x00,
}
