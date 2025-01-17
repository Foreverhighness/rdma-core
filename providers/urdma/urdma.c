/* SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause */

#include <stdlib.h>

#include "urdma.h"

static void urdma_free_context(struct ibv_context *ibctx);

static const struct verbs_context_ops urdma_ctx_ops = {
	.free_context = urdma_free_context,
};

static struct verbs_context *urdma_alloc_context(struct ibv_device *ibdev,
						 int cmd_fd, void *private_data)
{
	struct urdma_context *context;
	context = verbs_init_and_alloc_context(ibdev, cmd_fd, context,
					       verbs_ctx, RDMA_DRIVER_UNKNOWN);
	if (!context)
		return NULL;

	if (ibv_cmd_get_context(&context->verbs_ctx, NULL, 0, NULL, 0))
		goto err_out;

	verbs_info(&context->verbs_ctx, "alloc context\n");
	verbs_set_ops(&context->verbs_ctx, &urdma_ctx_ops);
	return &context->verbs_ctx;

err_out:
	verbs_err(&context->verbs_ctx, "failed to get context\n");
	verbs_uninit_context(&context->verbs_ctx);
	free(context);
	return NULL;
}

static void urdma_free_context(struct ibv_context *ibctx)
{
	struct urdma_context *context = to_uctx(ibctx);

	verbs_info(verbs_get_ctx(ibctx), "free context\n");
	verbs_uninit_context(&context->verbs_ctx);
	free(context);
}

static const struct verbs_match_ent match_table[] = {
	VERBS_DRIVER_ID(RDMA_DRIVER_UNKNOWN),
	VERBS_NAME_MATCH("urdma", NULL),
	{},
};

static struct verbs_device *
urdma_alloc_device(struct verbs_sysfs_dev *sysfs_dev)
{
	struct urdma_device *dev;

	dev = calloc(1, sizeof(*dev));
	if (!dev)
		return NULL;

	return &dev->verbs_dev;
}

static void urdma_uninit_device(struct verbs_device *verbs_dev)
{
	struct urdma_device *dev = to_udev(&verbs_dev->device);
	free(dev);
}

static const struct verbs_device_ops urdma_dev_ops = {
	.name = "urdma",
	.match_min_abi_version = 1,
	.match_max_abi_version = 1,
	.match_table = match_table,
	.alloc_device = urdma_alloc_device,
	.uninit_device = urdma_uninit_device,
	.alloc_context = urdma_alloc_context,
};
PROVIDER_DRIVER(urdma, urdma_dev_ops);
