// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v2/resources/customer_feed.proto

package resources

import (
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
	wrappers "github.com/golang/protobuf/ptypes/wrappers"
	common "google.golang.org/genproto/googleapis/ads/googleads/v2/common"
	enums "google.golang.org/genproto/googleapis/ads/googleads/v2/enums"
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

// A customer feed.
type CustomerFeed struct {
	// Immutable. The resource name of the customer feed.
	// Customer feed resource names have the form:
	//
	// `customers/{customer_id}/customerFeeds/{feed_id}`
	ResourceName string `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	// Immutable. The feed being linked to the customer.
	Feed *wrappers.StringValue `protobuf:"bytes,2,opt,name=feed,proto3" json:"feed,omitempty"`
	// Indicates which placeholder types the feed may populate under the connected
	// customer. Required.
	PlaceholderTypes []enums.PlaceholderTypeEnum_PlaceholderType `protobuf:"varint,3,rep,packed,name=placeholder_types,json=placeholderTypes,proto3,enum=google.ads.googleads.v2.enums.PlaceholderTypeEnum_PlaceholderType" json:"placeholder_types,omitempty"`
	// Matching function associated with the CustomerFeed.
	// The matching function is used to filter the set of feed items selected.
	// Required.
	MatchingFunction *common.MatchingFunction `protobuf:"bytes,4,opt,name=matching_function,json=matchingFunction,proto3" json:"matching_function,omitempty"`
	// Output only. Status of the customer feed.
	// This field is read-only.
	Status               enums.FeedLinkStatusEnum_FeedLinkStatus `protobuf:"varint,5,opt,name=status,proto3,enum=google.ads.googleads.v2.enums.FeedLinkStatusEnum_FeedLinkStatus" json:"status,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                                `json:"-"`
	XXX_unrecognized     []byte                                  `json:"-"`
	XXX_sizecache        int32                                   `json:"-"`
}

func (m *CustomerFeed) Reset()         { *m = CustomerFeed{} }
func (m *CustomerFeed) String() string { return proto.CompactTextString(m) }
func (*CustomerFeed) ProtoMessage()    {}
func (*CustomerFeed) Descriptor() ([]byte, []int) {
	return fileDescriptor_cd7590a7bcbec413, []int{0}
}

func (m *CustomerFeed) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_CustomerFeed.Unmarshal(m, b)
}
func (m *CustomerFeed) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_CustomerFeed.Marshal(b, m, deterministic)
}
func (m *CustomerFeed) XXX_Merge(src proto.Message) {
	xxx_messageInfo_CustomerFeed.Merge(m, src)
}
func (m *CustomerFeed) XXX_Size() int {
	return xxx_messageInfo_CustomerFeed.Size(m)
}
func (m *CustomerFeed) XXX_DiscardUnknown() {
	xxx_messageInfo_CustomerFeed.DiscardUnknown(m)
}

var xxx_messageInfo_CustomerFeed proto.InternalMessageInfo

func (m *CustomerFeed) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

func (m *CustomerFeed) GetFeed() *wrappers.StringValue {
	if m != nil {
		return m.Feed
	}
	return nil
}

func (m *CustomerFeed) GetPlaceholderTypes() []enums.PlaceholderTypeEnum_PlaceholderType {
	if m != nil {
		return m.PlaceholderTypes
	}
	return nil
}

func (m *CustomerFeed) GetMatchingFunction() *common.MatchingFunction {
	if m != nil {
		return m.MatchingFunction
	}
	return nil
}

func (m *CustomerFeed) GetStatus() enums.FeedLinkStatusEnum_FeedLinkStatus {
	if m != nil {
		return m.Status
	}
	return enums.FeedLinkStatusEnum_UNSPECIFIED
}

func init() {
	proto.RegisterType((*CustomerFeed)(nil), "google.ads.googleads.v2.resources.CustomerFeed")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v2/resources/customer_feed.proto", fileDescriptor_cd7590a7bcbec413)
}

var fileDescriptor_cd7590a7bcbec413 = []byte{
	// 557 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x8c, 0x54, 0xcd, 0x6e, 0xd3, 0x40,
	0x10, 0x96, 0x93, 0xb4, 0x12, 0xa6, 0x54, 0x8d, 0x4f, 0xa1, 0x2a, 0x34, 0x45, 0x8a, 0xc8, 0x85,
	0x5d, 0x64, 0x7e, 0x0e, 0xe6, 0x82, 0x8d, 0x68, 0x25, 0x04, 0x28, 0x72, 0x51, 0x90, 0x20, 0xc5,
	0xda, 0xd8, 0x13, 0xc7, 0xaa, 0x77, 0xd7, 0xf2, 0xda, 0x41, 0x15, 0xea, 0xcb, 0x70, 0xe4, 0x51,
	0xfa, 0x14, 0x3d, 0xf7, 0x11, 0x38, 0x20, 0xe4, 0xf5, 0xda, 0x49, 0x83, 0x4c, 0x7b, 0x1b, 0xcf,
	0x7c, 0xdf, 0x37, 0x3f, 0x3b, 0x63, 0xfd, 0x45, 0xc8, 0x79, 0x18, 0x03, 0x26, 0x81, 0xc0, 0xa5,
	0x59, 0x58, 0x0b, 0x13, 0xa7, 0x20, 0x78, 0x9e, 0xfa, 0x20, 0xb0, 0x9f, 0x8b, 0x8c, 0x53, 0x48,
	0xbd, 0x19, 0x40, 0x80, 0x92, 0x94, 0x67, 0xdc, 0x38, 0x28, 0xb1, 0x88, 0x04, 0x02, 0xd5, 0x34,
	0xb4, 0x30, 0x51, 0x4d, 0xdb, 0x7d, 0xd9, 0xa4, 0xec, 0x73, 0x4a, 0x39, 0xc3, 0x94, 0x64, 0xfe,
	0x3c, 0x62, 0xa1, 0x37, 0xcb, 0x99, 0x9f, 0x45, 0x9c, 0x95, 0xd2, 0xbb, 0xcf, 0x9b, 0x78, 0xc0,
	0x72, 0x2a, 0x70, 0x51, 0x84, 0x17, 0x47, 0xec, 0xd4, 0x13, 0x19, 0xc9, 0x72, 0x71, 0x3b, 0x56,
	0x12, 0x13, 0x1f, 0xe6, 0x3c, 0x0e, 0x20, 0xf5, 0xb2, 0xb3, 0x04, 0x14, 0x6b, 0xbf, 0x62, 0x25,
	0x11, 0x9e, 0x45, 0x10, 0x07, 0xde, 0x14, 0xe6, 0x64, 0x11, 0xf1, 0x54, 0x01, 0xee, 0xaf, 0x00,
	0xaa, 0xd6, 0x54, 0xe8, 0xa1, 0x0a, 0xc9, 0xaf, 0x69, 0x3e, 0xc3, 0xdf, 0x53, 0x92, 0x24, 0x90,
	0x56, 0x15, 0xed, 0xad, 0x50, 0x09, 0x63, 0x3c, 0x23, 0x45, 0x93, 0x2a, 0xfa, 0xe8, 0xa2, 0xa3,
	0x6f, 0xbd, 0x51, 0x83, 0x3d, 0x04, 0x08, 0x0c, 0x57, 0xbf, 0x57, 0x25, 0xf0, 0x18, 0xa1, 0xd0,
	0xd3, 0xfa, 0xda, 0xf0, 0x8e, 0xf3, 0xe4, 0xd2, 0xde, 0xf8, 0x6d, 0x3f, 0xd6, 0x07, 0xcb, 0x29,
	0x2b, 0x2b, 0x89, 0x04, 0xf2, 0x39, 0xc5, 0xab, 0x2a, 0xee, 0x56, 0xa5, 0xf1, 0x91, 0x50, 0x30,
	0x3e, 0xeb, 0x9d, 0x62, 0x5c, 0xbd, 0x56, 0x5f, 0x1b, 0xde, 0x35, 0xf7, 0x14, 0x13, 0x55, 0x15,
	0xa3, 0xe3, 0x2c, 0x8d, 0x58, 0x38, 0x26, 0x71, 0x0e, 0xce, 0x40, 0x26, 0xda, 0xd7, 0x1f, 0x34,
	0x26, 0x92, 0x09, 0xa4, 0xa0, 0xc1, 0xf5, 0xee, 0xfa, 0x44, 0x45, 0xaf, 0xdd, 0x6f, 0x0f, 0xb7,
	0x4d, 0x07, 0x35, 0xad, 0x86, 0x7c, 0x09, 0x34, 0x5a, 0xf2, 0x3e, 0x9d, 0x25, 0xf0, 0x96, 0xe5,
	0x74, 0xdd, 0xe7, 0xee, 0x24, 0xd7, 0x1d, 0xc2, 0x38, 0xd1, 0xbb, 0xff, 0xec, 0x4b, 0xaf, 0x23,
	0xdb, 0x7a, 0xda, 0x98, 0xb0, 0x5c, 0x34, 0xf4, 0x41, 0x11, 0x0f, 0x15, 0xcf, 0xdd, 0xa1, 0x6b,
	0x1e, 0xe3, 0x44, 0xdf, 0x2c, 0xb7, 0xa9, 0xb7, 0xd1, 0xd7, 0x86, 0xdb, 0xe6, 0xeb, 0x1b, 0x9a,
	0x28, 0x46, 0xf1, 0x3e, 0x62, 0xa7, 0xc7, 0x92, 0x24, 0x7b, 0xb8, 0xee, 0x72, 0xda, 0x97, 0x76,
	0xdb, 0x55, 0xa2, 0xd6, 0xb7, 0x2b, 0xfb, 0xeb, 0x2d, 0x5f, 0xd0, 0x30, 0xab, 0x73, 0x13, 0xf8,
	0x47, 0x65, 0x9e, 0xd7, 0x37, 0x58, 0x40, 0x56, 0x02, 0xf2, 0x24, 0xcf, 0x9d, 0x3f, 0x9a, 0x3e,
	0xf0, 0x39, 0x45, 0x37, 0x1e, 0xa5, 0xd3, 0x5d, 0xcd, 0x35, 0x2a, 0xf6, 0x60, 0xa4, 0x7d, 0x79,
	0xa7, 0x78, 0x21, 0x8f, 0x09, 0x0b, 0x11, 0x4f, 0x43, 0x1c, 0x02, 0x93, 0x5b, 0x82, 0x97, 0xa5,
	0xfe, 0xe7, 0x17, 0xf1, 0xaa, 0xb6, 0x7e, 0xb6, 0xda, 0x47, 0xb6, 0xfd, 0xab, 0x75, 0x70, 0x54,
	0x4a, 0xda, 0x81, 0x40, 0xa5, 0x59, 0x58, 0x63, 0x13, 0xb9, 0x15, 0xf2, 0xa2, 0xc2, 0x4c, 0xec,
	0x40, 0x4c, 0x6a, 0xcc, 0x64, 0x6c, 0x4e, 0x6a, 0xcc, 0x55, 0x6b, 0x50, 0x06, 0x2c, 0xcb, 0x0e,
	0x84, 0x65, 0xd5, 0x28, 0xcb, 0x1a, 0x9b, 0x96, 0x55, 0xe3, 0xa6, 0x9b, 0xb2, 0xd8, 0x67, 0x7f,
	0x03, 0x00, 0x00, 0xff, 0xff, 0xd4, 0xad, 0x53, 0xd9, 0xce, 0x04, 0x00, 0x00,
}
