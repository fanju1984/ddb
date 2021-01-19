// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v3/errors/asset_link_error.proto

package errors

import (
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
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

// Enum describing possible asset link errors.
type AssetLinkErrorEnum_AssetLinkError int32

const (
	// Enum unspecified.
	AssetLinkErrorEnum_UNSPECIFIED AssetLinkErrorEnum_AssetLinkError = 0
	// The received error code is not known in this version.
	AssetLinkErrorEnum_UNKNOWN AssetLinkErrorEnum_AssetLinkError = 1
	// Pinning is not supported for the given asset link field.
	AssetLinkErrorEnum_PINNING_UNSUPPORTED AssetLinkErrorEnum_AssetLinkError = 2
)

var AssetLinkErrorEnum_AssetLinkError_name = map[int32]string{
	0: "UNSPECIFIED",
	1: "UNKNOWN",
	2: "PINNING_UNSUPPORTED",
}

var AssetLinkErrorEnum_AssetLinkError_value = map[string]int32{
	"UNSPECIFIED":         0,
	"UNKNOWN":             1,
	"PINNING_UNSUPPORTED": 2,
}

func (x AssetLinkErrorEnum_AssetLinkError) String() string {
	return proto.EnumName(AssetLinkErrorEnum_AssetLinkError_name, int32(x))
}

func (AssetLinkErrorEnum_AssetLinkError) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_622441d6ee34c6fb, []int{0, 0}
}

// Container for enum describing possible asset link errors.
type AssetLinkErrorEnum struct {
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *AssetLinkErrorEnum) Reset()         { *m = AssetLinkErrorEnum{} }
func (m *AssetLinkErrorEnum) String() string { return proto.CompactTextString(m) }
func (*AssetLinkErrorEnum) ProtoMessage()    {}
func (*AssetLinkErrorEnum) Descriptor() ([]byte, []int) {
	return fileDescriptor_622441d6ee34c6fb, []int{0}
}

func (m *AssetLinkErrorEnum) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_AssetLinkErrorEnum.Unmarshal(m, b)
}
func (m *AssetLinkErrorEnum) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_AssetLinkErrorEnum.Marshal(b, m, deterministic)
}
func (m *AssetLinkErrorEnum) XXX_Merge(src proto.Message) {
	xxx_messageInfo_AssetLinkErrorEnum.Merge(m, src)
}
func (m *AssetLinkErrorEnum) XXX_Size() int {
	return xxx_messageInfo_AssetLinkErrorEnum.Size(m)
}
func (m *AssetLinkErrorEnum) XXX_DiscardUnknown() {
	xxx_messageInfo_AssetLinkErrorEnum.DiscardUnknown(m)
}

var xxx_messageInfo_AssetLinkErrorEnum proto.InternalMessageInfo

func init() {
	proto.RegisterEnum("google.ads.googleads.v3.errors.AssetLinkErrorEnum_AssetLinkError", AssetLinkErrorEnum_AssetLinkError_name, AssetLinkErrorEnum_AssetLinkError_value)
	proto.RegisterType((*AssetLinkErrorEnum)(nil), "google.ads.googleads.v3.errors.AssetLinkErrorEnum")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v3/errors/asset_link_error.proto", fileDescriptor_622441d6ee34c6fb)
}

var fileDescriptor_622441d6ee34c6fb = []byte{
	// 301 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x7c, 0x90, 0xc1, 0x4a, 0xc3, 0x30,
	0x1c, 0xc6, 0x5d, 0x05, 0x85, 0x0c, 0x74, 0x74, 0x07, 0x41, 0x64, 0x87, 0x3e, 0x40, 0x72, 0x08,
	0x5e, 0xe2, 0x29, 0xb3, 0xb5, 0x14, 0x25, 0x0b, 0xce, 0x56, 0x90, 0x4a, 0x89, 0xb6, 0x84, 0xb2,
	0x2e, 0x29, 0x4d, 0xdd, 0x03, 0x79, 0xf4, 0x51, 0x7c, 0x14, 0x0f, 0x3e, 0x83, 0xb4, 0xb1, 0x85,
	0x1d, 0xf4, 0x94, 0x8f, 0x8f, 0xdf, 0xf7, 0xe5, 0xe3, 0x0f, 0x2e, 0xa5, 0xd6, 0xb2, 0x2a, 0x90,
	0xc8, 0x0d, 0xb2, 0xb2, 0x53, 0x3b, 0x8c, 0x8a, 0xa6, 0xd1, 0x8d, 0x41, 0xc2, 0x98, 0xa2, 0xcd,
	0xaa, 0x52, 0x6d, 0xb2, 0xde, 0x81, 0x75, 0xa3, 0x5b, 0xed, 0x2e, 0x2c, 0x0b, 0x45, 0x6e, 0xe0,
	0x18, 0x83, 0x3b, 0x0c, 0x6d, 0xec, 0xfc, 0x62, 0xa8, 0xad, 0x4b, 0x24, 0x94, 0xd2, 0xad, 0x68,
	0x4b, 0xad, 0x8c, 0x4d, 0x7b, 0xcf, 0xc0, 0xa5, 0x5d, 0xef, 0x5d, 0xa9, 0x36, 0x41, 0x17, 0x08,
	0xd4, 0xdb, 0xd6, 0x0b, 0xc1, 0xc9, 0xbe, 0xeb, 0x9e, 0x82, 0x69, 0xcc, 0xd6, 0x3c, 0xb8, 0x8e,
	0x6e, 0xa2, 0xc0, 0x9f, 0x1d, 0xb8, 0x53, 0x70, 0x1c, 0xb3, 0x5b, 0xb6, 0x7a, 0x64, 0xb3, 0x89,
	0x7b, 0x06, 0xe6, 0x3c, 0x62, 0x2c, 0x62, 0x61, 0x16, 0xb3, 0x75, 0xcc, 0xf9, 0xea, 0xfe, 0x21,
	0xf0, 0x67, 0xce, 0xf2, 0x7b, 0x02, 0xbc, 0x57, 0xbd, 0x85, 0xff, 0x6f, 0x5c, 0xce, 0xf7, 0x7f,
	0xe3, 0xdd, 0x34, 0x3e, 0x79, 0xf2, 0x7f, 0x63, 0x52, 0x57, 0x42, 0x49, 0xa8, 0x1b, 0x89, 0x64,
	0xa1, 0xfa, 0xe1, 0xc3, 0x85, 0xea, 0xd2, 0xfc, 0x75, 0xb0, 0x2b, 0xfb, 0xbc, 0x3b, 0x87, 0x21,
	0xa5, 0x1f, 0xce, 0x22, 0xb4, 0x65, 0x34, 0x37, 0xd0, 0xca, 0x4e, 0x25, 0x18, 0xf6, 0x5f, 0x9a,
	0xcf, 0x01, 0x48, 0x69, 0x6e, 0xd2, 0x11, 0x48, 0x13, 0x9c, 0x5a, 0xe0, 0xcb, 0xf1, 0xac, 0x4b,
	0x08, 0xcd, 0x0d, 0x21, 0x23, 0x42, 0x48, 0x82, 0x09, 0xb1, 0xd0, 0xcb, 0x51, 0xbf, 0x0e, 0xff,
	0x04, 0x00, 0x00, 0xff, 0xff, 0xbc, 0x10, 0xa6, 0x05, 0xcd, 0x01, 0x00, 0x00,
}
