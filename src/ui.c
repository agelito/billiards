// ui.c

void
ui_draw_label(game_state* state, vector2 position, char* text, real32 size, loaded_font* font)
{
    matrix4 text_transform = matrix_translate(position.x, position.y, 0.0f);

    vector2 text_size = font_measure_text(&font->data, size, text);
    
    matrix4 text_background_translate =
	matrix_translate(position.x + text_size.x * 0.5f, position.y + text_size.y * 0.5f, 0.0f);
    matrix4 text_background_scale =
	matrix_scale(text_size.x + 10.0f, text_size.y + 10.0f, 1.0f);
    matrix4 text_background_transform =
	matrix_multiply(text_background_translate, text_background_scale);

    renderer_queue_push_draw(&state->render_queue, &state->quad,
			     &state->text_background, text_background_transform);
    renderer_queue_push_text(&state->render_queue, text, font,
			     size, &state->text, text_transform);
}
