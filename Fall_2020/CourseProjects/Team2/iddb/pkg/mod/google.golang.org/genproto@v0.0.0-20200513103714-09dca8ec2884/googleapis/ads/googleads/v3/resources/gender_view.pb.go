// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v3/resources/gender_view.proto

package resources

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

// A gender view.
type GenderView struct {
	// Output only. The resource name of the gender view.
	// Gender view resource names have the form:
	//
	// `customers/{customer_id}/genderViews/{ad_group_id}~{criterion_id}`
	ResourceName         string   `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *GenderView) Reset()         { *m = GenderView{} }
func (m *GenderView) String() string { return proto.CompactTextString(m) }
func (*GenderView) ProtoMessage()    {}
func (*GenderView) Descriptor() ([]byte, []int) {
	return fileDescriptor_374a728b177a8cfa, []int{0}
}

func (m *GenderView) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_GenderView.Unmarshal(m, b)
}
func (m *GenderView) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_GenderView.Marshal(b, m, deterministic)
}
func (m *GenderView) XXX_Merge(src proto.Message) {
	xxx_messageInfo_GenderView.Merge(m, src)
}
func (m *GenderView) XXX_Size() int {
	return xxx_messageInfo_GenderView.Size(m)
}
func (m *GenderView) XXX_DiscardUnknown() {
	xxx_messageInfo_GenderView.DiscardUnknown(m)
}

var xxx_messageInfo_GenderView proto.InternalMessageInfo

func (m *GenderView) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

func init() {
	proto.RegisterType((*GenderView)(nil), "google.ads.googleads.v3.resources.GenderView")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v3/resources/gender_view.proto", fileDescriptor_374a728b177a8cfa)
}

var fileDescriptor_374a728b177a8cfa = []byte{
	// 341 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x8c, 0x91, 0x41, 0x4b, 0xf3, 0x30,
	0x1c, 0xc6, 0xe9, 0x06, 0x2f, 0xbc, 0x45, 0x11, 0x76, 0xd2, 0x21, 0xe8, 0x94, 0x81, 0x20, 0x24,
	0x87, 0xdc, 0xe2, 0x29, 0xbb, 0x0c, 0x3c, 0xc8, 0x18, 0x58, 0x44, 0x0a, 0x23, 0x6b, 0x62, 0x0d,
	0xac, 0xfd, 0x8f, 0xa4, 0xeb, 0x0e, 0x63, 0x5f, 0xc6, 0xa3, 0x9f, 0xc0, 0xcf, 0xe0, 0xa7, 0xf0,
	0xbc, 0x8f, 0x20, 0x1e, 0xa4, 0x4b, 0x93, 0xf6, 0xa4, 0xde, 0x1e, 0xf8, 0xff, 0x9e, 0x27, 0x4f,
	0x9f, 0x86, 0x24, 0x05, 0x48, 0x17, 0x12, 0x73, 0x61, 0xb0, 0x95, 0x95, 0x2a, 0x09, 0xd6, 0xd2,
	0xc0, 0x4a, 0x27, 0xd2, 0xe0, 0x54, 0xe6, 0x42, 0xea, 0x59, 0xa9, 0xe4, 0x1a, 0x2d, 0x35, 0x14,
	0xd0, 0x1b, 0x58, 0x12, 0x71, 0x61, 0x90, 0x37, 0xa1, 0x92, 0x20, 0x6f, 0xea, 0x9f, 0xb9, 0xdc,
	0xa5, 0xc2, 0x4f, 0x4a, 0x2e, 0xc4, 0x6c, 0x2e, 0x9f, 0x79, 0xa9, 0x40, 0xdb, 0x8c, 0xfe, 0x49,
	0x0b, 0x70, 0xb6, 0xfa, 0x74, 0xda, 0x3a, 0xf1, 0x3c, 0x87, 0x82, 0x17, 0x0a, 0x72, 0x63, 0xaf,
	0x17, 0x6f, 0x41, 0x18, 0x8e, 0xf7, 0x95, 0x22, 0x25, 0xd7, 0xbd, 0x49, 0x78, 0xe8, 0xec, 0xb3,
	0x9c, 0x67, 0xf2, 0x38, 0x38, 0x0f, 0xae, 0xfe, 0x8f, 0xae, 0x3f, 0x58, 0xf7, 0x93, 0x0d, 0xc3,
	0xcb, 0xa6, 0x5f, 0xad, 0x96, 0xca, 0xa0, 0x04, 0x32, 0xdc, 0x64, 0x4c, 0x0f, 0x5c, 0xc2, 0x1d,
	0xcf, 0x24, 0x7d, 0xd8, 0xb1, 0xfb, 0x3f, 0xf9, 0x7a, 0x28, 0x59, 0x99, 0x02, 0x32, 0xa9, 0x0d,
	0xde, 0x38, 0xb9, 0xad, 0xf7, 0xaa, 0x00, 0x83, 0x37, 0xad, 0xf1, 0xb6, 0xa3, 0xaf, 0x20, 0x1c,
	0x26, 0x90, 0xa1, 0x5f, 0xe7, 0x1b, 0x1d, 0x35, 0xaf, 0x4c, 0xaa, 0xaf, 0x9e, 0x04, 0x8f, 0xb7,
	0xb5, 0x2b, 0x85, 0x05, 0xcf, 0x53, 0x04, 0x3a, 0xad, 0x5e, 0xda, 0x6f, 0x82, 0x9b, 0x8a, 0x3f,
	0xfc, 0xc8, 0x1b, 0xaf, 0x5e, 0x3a, 0xdd, 0x31, 0x63, 0xaf, 0x9d, 0xc1, 0xd8, 0x46, 0x32, 0x61,
	0x90, 0x95, 0x95, 0x8a, 0x08, 0x9a, 0x3a, 0xf2, 0xdd, 0x31, 0x31, 0x13, 0x26, 0xf6, 0x4c, 0x1c,
	0x91, 0xd8, 0x33, 0xbb, 0xce, 0xd0, 0x1e, 0x28, 0x65, 0xc2, 0x50, 0xea, 0x29, 0x4a, 0x23, 0x42,
	0xa9, 0xe7, 0xe6, 0xff, 0xf6, 0x65, 0xc9, 0x77, 0x00, 0x00, 0x00, 0xff, 0xff, 0x15, 0x60, 0x0f,
	0x59, 0x74, 0x02, 0x00, 0x00,
}