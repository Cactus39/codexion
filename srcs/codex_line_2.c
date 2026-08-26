/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex_line_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsutormi <dsutormi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:47:35 by dsutormi          #+#    #+#             */
/*   Updated: 2026/08/25 18:47:42 by dsutormi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	ft_run_search(t_node *node, t_settings *settings, t_coder coders[])
{
	if (node == NULL)
		return (0);
	if (ft_run_search(node->left, settings, coders))
		return (1);
	if (coders[node->id].ready == 0 && ft_validate(&coders[node->id]))
	{
		pthread_mutex_lock(&(coders[node->id].status_mtx));
		coders[node->id].ready = 1;
		pthread_mutex_unlock(&(coders[node->id].status_mtx));
		ft_delrestruct_tree(&settings->fifo_head, node);
		return (1);
	}
	return (ft_run_search(node->right, settings, coders));
}

static void	run(t_node *node, t_settings *settings, t_coder coders[])
{
	t_node		*last_node;
	int			found;

	pthread_mutex_lock(&settings->line_mtx);
	found = ft_run_search(node, settings, coders);
	pthread_mutex_unlock(&settings->line_mtx);
	if (found)
		pthread_cond_broadcast(&settings->sig);
}

void	*ft_run_fifo_2(void *coders_v)
{
	t_coder		*coders;
	t_settings	*settings;

	coders = (t_coder *)(coders_v);
	settings = coders[0].s;
	while (settings->pr_stat)
	{
		if (settings->pr_stat && settings->fifo_head)
		{
			run(settings->fifo_head, settings, coders);
		}
		usleep(10);
	}
	return (NULL);
}

static int	ft_check_head(t_node **head, t_node *node)
{
	if (*head == NULL)
	{
		if (node->parent)
		{
			if (node->parent->left == node)
				node->parent->left = NULL;
			if (node->parent->right == node)
				node->parent->right = NULL;
		}
		*head = node;
		node->parent = NULL;
		return (1);
	}
	return (0);
}

void	ft_append_tree(t_node **head, t_node *node)
{
	if (node == NULL)
		return ;
	if (ft_check_head(head, node))
		return ;
	if (node->value > (*head)->value && (*head)->right == NULL)
	{
		(*head)->right = node;
		node->parent = *head;
		node->left = NULL;
		node->right = NULL;
		return ;
	}
	if (node->value <= (*head)->value && (*head)->left == NULL)
	{
		(*head)->left = node;
		node->parent = *head;
		node->left = NULL;
		node->right = NULL;
		return ;
	}
	if ((node->value > (*head)->value) && ((*head)->right != NULL))
		return (ft_append_tree(&((*head)->right), node));
	if ((node->value <= (*head)->value) && ((*head)->left != NULL))
		return (ft_append_tree(&((*head)->left), node));
}
