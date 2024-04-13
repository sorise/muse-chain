## [CA、P2P组网相关网络接口](#)

### CA 接口
RPC请求接口登记表。

| 路由                | 说明                      | 返回                   |
|:------------------|:------------------------|:---------------------|
| @context/ca/all   | 请求CA结点获取网络中其他结点的相关授权信息。 | 返回一个列表，说明结点的公钥、授权时长。 |
| @context/ca/count | 请求CA结点总的结点数量。           | 返回数字                 |
| @context/ca/apply | 向CA结点申请结点授权。            | 返回是否授权，授权时长。         |
| @context/ca/renew | 向CA结点申请继续授权。            | 返回是否延长授权，授权时长。       |
| @context/ca/certificate   | 向CA结点申请自己的授权证书。         | 获得授权证书。              |
| @context/ca/cancel   | 向CA结点申请停止授权。            | 停止授权。                |

### P2P 组网接口


| 路由                               | 说明             | 返回        |
|:---------------------------------|:---------------|:----------|
| @context/routing_center/register | 向路由注册自己的网络地址   | 返回结点的网络地址 |
| @context/routing_center/search   | 请求网络中其他结点的网络地址 | 返回结点的网络地址 |
| @context/routing_center/all      | 请求所有的其他结点的网络地址 | 返回所有的结点的网络地址 |