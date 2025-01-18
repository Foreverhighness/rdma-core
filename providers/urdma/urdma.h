/* SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause */

#ifndef __URDMA_H__
#define __URDMA_H__

#include <infiniband/driver.h>
#include <infiniband/verbs.h>

struct urdma_device {
	struct verbs_device verbs_dev;
	void* driver_data;
};

struct urdma_context {
	struct verbs_context verbs_ctx;
};

static inline struct urdma_context *to_uctx(struct ibv_context *ibctx)
{
	return container_of(ibctx, struct urdma_context, verbs_ctx.context);
}

static inline struct urdma_device *to_udev(struct ibv_device *ibdev)
{
	return container_of(ibdev, struct urdma_device, verbs_dev.device);
}

#endif /* __URDMA_H__ */
