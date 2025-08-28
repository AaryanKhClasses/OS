[bits 32]
global ctx_switch

ctx_switch:
    push ebp
    push ebx
    push esi
    push edi
    ; stack layout: edi <- esi <- ebx <- ebp <- return_eip <- old_esp <- new_esp

    mov eax, [esp + 20]     ; eax = old_esp_ptr
    mov [eax], esp      ; value(old_esp_ptr) = esp
    mov esp, [esp + 24]     ; esp = new_esp_ptr

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
