// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v3/resources/ad_group_ad_label.proto

package resources

import (
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
	wrappers "github.com/golang/protobuf/ptypes/wrappers"
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

// A relationship between an ad group ad and a label.
type AdGroupAdLabel struct {
	// Immutable. The resource name of the ad group ad label.
	// Ad group ad label resource names have the form:
	// `customers/{customer_id}/adGroupAdLabels/{ad_group_id}~{ad_id}~{label_id}`
	ResourceName string `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	// Immutable. The ad group ad to which the label is attached.
	AdGroupAd *wrappers.StringValue `protobuf:"bytes,2,opt,name=ad_group_ad,json=adGroupAd,proto3" json:"ad_group_ad,omitempty"`
	// Immutable. The label assigned to the ad group ad.
	Label                *wrappers.StringValue `protobuf:"bytes,3,opt,name=label,proto3" json:"label,omitempty"`
	XXX_NoUnkeyedLiteral struct{}              `json:"-"`
	XXX_unrecognized     []byte                `json:"-"`
	XXX_sizecache        int32                 `json:"-"`
}

func (m *AdGroupAdLabel) Reset()         { *m = AdGroupAdLabel{} }
func (m *AdGroupAdLabel) String() string { return proto.CompactTextString(m) }
func (*AdGroupAdLabel) ProtoMessage()    {}
func (*AdGroupAdLabel) Descriptor() ([]byte, []int) {
	return fileDescriptor_668144ba7a9ba42f, []int{0}
}

func (m *AdGroupAdLabel) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_AdGroupAdLabel.Unmarshal(m, b)
}
func (m *AdGroupAdLabel) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_AdGroupAdLabel.Marshal(b, m, deterministic)
}
func (m *AdGroupAdLabel) XXX_Merge(src proto.Message) {
	xxx_messageInfo_AdGroupAdLabel.Merge(m, src)
}
func (m *AdGroupAdLabel) XXX_Size() int {
	return xxx_messageInfo_AdGroupAdLabel.Size(m)
}
func (m *AdGroupAdLabel) XXX_DiscardUnknown() {
	xxx_messageInfo_AdGroupAdLabel.DiscardUnknown(m)
}

var xxx_messageInfo_AdGroupAdLabel proto.InternalMessageInfo

func (m *AdGroupAdLabel) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

func (m *AdGroupAdLabel) GetAdGroupAd() *wrappers.StringValue {
	if m != nil {
		return m.AdGroupAd
	}
	return nil
}

func (m *AdGroupAdLabel) GetLabel() *wrappers.StringValue {
	if m != nil {
		return m.Label
	}
	return nil
}

func init() {
	proto.RegisterType((*AdGroupAdLabel)(nil), "google.ads.googleads.v3.resources.AdGroupAdLabel")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v3/resources/ad_group_ad_label.proto", fileDescriptor_668144ba7a9ba42f)
}

var fileDescriptor_668144ba7a9ba42f = []byte{
	// 431 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x8c, 0x52, 0xcf, 0x8a, 0xd4, 0x30,
	0x18, 0xa7, 0x1d, 0x56, 0xd8, 0xac, 0x7a, 0xa8, 0x97, 0x71, 0x59, 0xd6, 0xd9, 0xc5, 0xd5, 0xd5,
	0x43, 0x02, 0xf6, 0xa2, 0xf1, 0x94, 0x5e, 0x06, 0x44, 0x64, 0x19, 0xa5, 0x87, 0xa5, 0x50, 0xd2,
	0x26, 0x93, 0x2d, 0xb4, 0x4d, 0x49, 0xda, 0xf1, 0x30, 0x0c, 0xf8, 0x10, 0x3e, 0x81, 0x47, 0x1f,
	0xc5, 0xa7, 0xf0, 0x3c, 0x8f, 0xe0, 0x49, 0xda, 0x34, 0x9d, 0x19, 0xc4, 0x9d, 0xb9, 0xfd, 0xc2,
	0xf7, 0xfb, 0x97, 0x8f, 0x0f, 0xbc, 0x13, 0x52, 0x8a, 0x9c, 0x23, 0xca, 0x34, 0x32, 0xb0, 0x45,
	0x0b, 0x1f, 0x29, 0xae, 0x65, 0xa3, 0x52, 0xae, 0x11, 0x65, 0xb1, 0x50, 0xb2, 0xa9, 0x62, 0xca,
	0xe2, 0x9c, 0x26, 0x3c, 0x87, 0x95, 0x92, 0xb5, 0xf4, 0x2e, 0x0c, 0x1f, 0x52, 0xa6, 0xe1, 0x20,
	0x85, 0x0b, 0x1f, 0x0e, 0xd2, 0xd3, 0x67, 0xd6, 0xbd, 0xca, 0xd0, 0x3c, 0xe3, 0x39, 0x8b, 0x13,
	0x7e, 0x47, 0x17, 0x99, 0x54, 0xc6, 0xe3, 0xf4, 0xe9, 0x16, 0xc1, 0xca, 0xfa, 0xd1, 0x79, 0x3f,
	0xea, 0x5e, 0x49, 0x33, 0x47, 0x5f, 0x15, 0xad, 0x2a, 0xae, 0x74, 0x3f, 0x3f, 0xdb, 0x92, 0xd2,
	0xb2, 0x94, 0x35, 0xad, 0x33, 0x59, 0xf6, 0xd3, 0xcb, 0xef, 0x23, 0xf0, 0x98, 0xb0, 0x69, 0xdb,
	0x9b, 0xb0, 0x8f, 0x6d, 0x6b, 0xef, 0x0b, 0x78, 0x64, 0x23, 0xe2, 0x92, 0x16, 0x7c, 0xec, 0x4c,
	0x9c, 0xeb, 0xe3, 0x00, 0xfd, 0x26, 0x47, 0x7f, 0xc8, 0x2b, 0xf0, 0x72, 0xf3, 0x87, 0x1e, 0x55,
	0x99, 0x86, 0xa9, 0x2c, 0xd0, 0xae, 0xcf, 0xec, 0xa1, 0x75, 0xf9, 0x44, 0x0b, 0xee, 0xdd, 0x81,
	0x93, 0xad, 0x05, 0x8d, 0xdd, 0x89, 0x73, 0x7d, 0xf2, 0xe6, 0xac, 0xb7, 0x80, 0xb6, 0x3c, 0xfc,
	0x5c, 0xab, 0xac, 0x14, 0x21, 0xcd, 0x1b, 0x1e, 0xbc, 0xee, 0x12, 0x9f, 0x83, 0xcb, 0xfd, 0x89,
	0xb3, 0x63, 0x6a, 0xa1, 0x77, 0x0b, 0x8e, 0xba, 0xf5, 0x8f, 0x47, 0x07, 0x64, 0xbc, 0xe8, 0x32,
	0x26, 0xe0, 0xfc, 0xbf, 0x19, 0xe6, 0x33, 0xc6, 0x12, 0xcf, 0xd7, 0x24, 0x3d, 0x78, 0x03, 0xde,
	0xdb, 0xb4, 0xd1, 0xb5, 0x2c, 0xb8, 0xd2, 0x68, 0x69, 0xe1, 0x0a, 0xd1, 0x1d, 0x92, 0x46, 0xcb,
	0x7f, 0x0e, 0x67, 0x15, 0x7c, 0x73, 0xc1, 0x55, 0x2a, 0x0b, 0xb8, 0xf7, 0x74, 0x82, 0x27, 0xbb,
	0x99, 0x37, 0xed, 0x27, 0x6f, 0x9c, 0xdb, 0x0f, 0xbd, 0x52, 0xc8, 0x9c, 0x96, 0x02, 0x4a, 0x25,
	0x90, 0xe0, 0x65, 0xb7, 0x02, 0xb4, 0x29, 0x7d, 0xcf, 0x39, 0xbf, 0x1f, 0xd0, 0x0f, 0x77, 0x34,
	0x25, 0xe4, 0xa7, 0x7b, 0x31, 0x35, 0x96, 0x84, 0x69, 0x68, 0x60, 0x8b, 0x42, 0x1f, 0xce, 0x2c,
	0xf3, 0x97, 0xe5, 0x44, 0x84, 0xe9, 0x68, 0xe0, 0x44, 0xa1, 0x1f, 0x0d, 0x9c, 0xb5, 0x7b, 0x65,
	0x06, 0x18, 0x13, 0xa6, 0x31, 0x1e, 0x58, 0x18, 0x87, 0x3e, 0xc6, 0x03, 0x2f, 0x79, 0xd0, 0x95,
	0xf5, 0xff, 0x06, 0x00, 0x00, 0xff, 0xff, 0x2b, 0x78, 0x4b, 0xda, 0x7a, 0x03, 0x00, 0x00,
}
